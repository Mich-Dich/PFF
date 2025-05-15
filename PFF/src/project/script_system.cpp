
#include "util/pffpch.h"

#include <entt/entt.hpp>

#include "application.h"
#include "util/io/serializer_yaml.h"
#include "util/io/io.h"
#include "util/system.h"

#include "engine/layer/world_layer.h"
#include "engine/resource_management/headers.h"
#include "engine/world/map.h"
#include "engine/world/entity.h"

#if defined(PFF_PLATFORM_WINDOWS)
	#include <Windows.h>
#elif defined(PFF_PLATFORM_LINUX)
	#include <dlfcn.h> // For dynamic library loading on Linux
#else
	#error Platform not supported
#endif

#include "script_system.h"


namespace PFF::script_system {

#define GET_MAP																	application::get().get_world_layer()->get_map()
#define GET_REGISTRY_OF_MAP 													GET_MAP->get_registry()
#define TRY_EXECUTE_FUNCTION(name) 												if (name) name

#if defined(__GNUC__) && !defined(__clang__)
#elif defined(__clang__)
    #error "Clang not jet supported"
#elif defined(_MSC_VER)
#else
    #error "Unsupported compiler!"
#endif


#if defined(PFF_PLATFORM_WINDOWS)

	static HMODULE			m_module;

	static FARPROC __stdcall try_load_function(const char* functionName) {

		auto func = GetProcAddress(m_module, functionName);
		VALIDATE(func, , "successfully loaded function [" << functionName << "]", "Could not load dll function [" << functionName << "]");
		return func;
	}

	static bool load_library(std::filesystem::path path) {

		m_module = LoadLibraryA(path.string().c_str());
		return m_module;
	}

	static bool free_library() {

		if (!FreeLibrary(m_module)) {
			DWORD errorCode = GetLastError();
			LOG(Warn, "Could not free script dll. Error Code: " << errorCode);
			return false;
		}

		m_module = nullptr;
		return true;
	}

#elif defined(PFF_PLATFORM_LINUX)

	static void* m_module; // Use void* for the module handle

	static void* try_load_function(const char* functionName) {
		void* func = dlsym(m_module, functionName);
		VALIDATE(func, , "successfully loaded function [" << functionName << "]", "Could not load shared library function [" << functionName << "]");
		return func;
	}

	static bool load_library(std::filesystem::path path) {
		m_module = dlopen(path.string().c_str(), RTLD_LAZY);
		return m_module != nullptr;
	}

	static bool free_library() {
		if (dlclose(m_module) != 0) {
			LOG(Warn, "Could not free script shared library. Error: " << dlerror());
			return false;
		}
		m_module = nullptr;
		return true;
	}

#endif

	
	typedef void (*serialize_script_Fn)(std::string, entity_script*, serializer::yaml&);
	typedef void (*init_scripts_Fn)(entt::registry*, ImGuiContext*);
	typedef void (*delete_scripts_Fn)();
	typedef void (*add_component_Fn)(std::string, PFF::entity);
	typedef void (*display_properties_Fn)(std::string, entity_script*);
	typedef const char** (*get_scripts_Fn)(u32* count);

	static const char*																						could_not_load_scripts[] = { nullptr };
	static bool																								m_is_loaded = false;
	static bool																								m_is_initalized = false;
	static add_component_Fn																					m_add_component_from_string = nullptr;
	static get_scripts_Fn																					m_get_scripts = nullptr;
	static get_scripts_Fn																					m_get_procedural_mesh_scripts = nullptr;
	static display_properties_Fn																			m_display_properties = nullptr;
	static init_scripts_Fn																					m_init_scripts = nullptr;
	static serialize_script_Fn																				m_serialize_script = nullptr;

	static void add_component_stub(std::string, PFF::entity)												{ LOG(Warn, "Adding component from STUB"); }
	void shutdown()																							{ free_script_library(true); }
	void add_component_from_string(std::string class_name, PFF::entity entity)								{ LOG(Warn, "Trying to load a component from name [" << class_name << "]") TRY_EXECUTE_FUNCTION(m_add_component_from_string)(class_name, entity); }
	void display_properties(std::string class_name, entity_script* script)									{ TRY_EXECUTE_FUNCTION(m_display_properties)(class_name, script); }
	void serialize_script(std::string class_name, entity_script* script, serializer::yaml& serializer)		{ TRY_EXECUTE_FUNCTION(m_serialize_script)(class_name, script, serializer); }
	bool is_ready()																							{ return m_is_loaded && m_is_initalized; }
	const char** get_all_procedural_mesh_scripts(u32* count)												{ return (m_get_procedural_mesh_scripts) ? m_get_procedural_mesh_scripts(count) : could_not_load_scripts; }
	const char** get_all_scripts(u32* count)																{ return (m_get_scripts) ? m_get_scripts(count) : could_not_load_scripts; }


	static void serialize_scripts(serializer::option option) {

		std::filesystem::path script_components_serializer_file = PROJECT_PATH / CONFIG_DIR / "script_components.yml";
		LOG(Trace, "serializing script components to: [" << script_components_serializer_file.generic_string() << "]");

		asset_file_header file_header{};
		file_header.file_version = version(1, 0, 0);
		file_header.type = file_type::world;
		file_header.timestamp = util::get_system_time();

		auto serializer = serializer::yaml(script_components_serializer_file, "script_components", option);
		serializer.sub_section("file_header", [&](serializer::yaml& header_section) {

			header_section.entry(KEY_VALUE(file_header.file_version))
				.entry(KEY_VALUE(file_header.type))
				.entry(KEY_VALUE(file_header.timestamp));
		});


		if (option == serializer::option::save_to_file) {

			const auto view = GET_REGISTRY_OF_MAP.view<procedural_mesh_component>();
			std::vector<entt::entity> entities(view.begin(), view.end());			// convert to vector<> because of serializer API
			std::reverse(entities.begin(), entities.end());							// reverse because EnTT counts from last to first in view<>()

			serializer.vector("entities", entities, [&](serializer::yaml& entity_section, u64 x) {
			
				entity loc_entity = PFF::entity(entities[x], GET_MAP.get());

				auto& name_comp = loc_entity.get_component<name_component>();
				entity_section.entry(KEY_VALUE(name_comp.name));

				auto& ID_comp = loc_entity.get_component<ID_component>();
				entity_section.entry(KEY_VALUE(ID_comp.ID));

				if (loc_entity.has_component<procedural_mesh_component>()) {

					auto& procedural_mesh_comp = loc_entity.get_component<procedural_mesh_component>(); 
					entity_section.sub_section("procedural_mesh_component", [&](serializer::yaml& component_section) {

						component_section.entry(KEY_VALUE(procedural_mesh_comp.script_name))
						.entry(KEY_VALUE(procedural_mesh_comp.mobility_data))
						.entry(KEY_VALUE(procedural_mesh_comp.shoudl_render));
						
						serialize_script(procedural_mesh_comp.script_name, (PFF::entity_script*)procedural_mesh_comp.instance, serializer);
					});
				};

			});

		} else {

			std::vector<entt::entity> entities{};

			serializer.vector("entities", entities, [&](serializer::yaml& entity_section, u64 x) {
				
				std::string name{};
				entity_section.entry(KEY_VALUE(name));

				UUID ID{};
				entity_section.entry(KEY_VALUE(ID));

				entity loc_entity = GET_MAP->create_entity_with_UUID(ID, name);

				entity_section.sub_section("procedural_mesh_component", [&](serializer::yaml& component_section) {

					procedural_mesh_component proc_mesh_comp{};
					entity_section.entry(KEY_VALUE(proc_mesh_comp.script_name))
						.entry(KEY_VALUE(proc_mesh_comp.mobility_data))
						.entry(KEY_VALUE(proc_mesh_comp.shoudl_render));

					serialize_script(proc_mesh_comp.script_name, (PFF::entity_script*)proc_mesh_comp.instance, serializer);
					add_component_from_string(proc_mesh_comp.script_name, loc_entity);

				});

			});

		}
	}


	void reinit_scripts() {					// TODO: causes crash => FIX that

		VALIDATE(m_init_scripts, return, "", "function [init_scripts()] is not available => could not reinit_scripts");
		VALIDATE(GET_MAP, return, "", "Failed to get map refrence => could not reinit_scripts");

		LOG(Debug, "Type ID of procedural_mesh_component before reinit: " << entt::type_hash<procedural_mesh_component>::value());
		m_init_scripts(&GET_REGISTRY_OF_MAP, ImGui::GetCurrentContext());
		LOG(Debug, "Type ID of procedural_mesh_component after reinit: " << entt::type_hash<procedural_mesh_component>::value());

		auto& registry = GET_REGISTRY_OF_MAP;
		LOG(Info, "Registry address in reinit_scripts: " << &registry);

		m_is_initalized = true;

		LOG(Info, "succesfully initalized scripts");
	}

	void init() {
		
		// LOG(Debug, "called script_system::init(delete_script_components=" << util::to_string(delete_script_components) << ")");
		// VALIDATE(free_script_library(delete_script_components), return, "", "Count not free scripting library");
		
		// auto& registry = GET_REGISTRY_OF_MAP;
		// LOG(Info, "Registry address in init: " << &registry);
	
		const std::filesystem::path project_dll_path = PROJECT_PATH / "bin" / PROJECT_NAME;
#if defined(PFF_PLATFORM_WINDOWS)
		const std::filesystem::path project_temp_dll = PROJECT_PATH / "bin" / (PROJECT_NAME + PFF_PROJECT_TEMP_DLL_PATH) / (PROJECT_NAME + ".dll");
		const std::filesystem::path project_dll = project_dll_path / (PROJECT_NAME + ".dll");
#elif defined(PFF_PLATFORM_LINUX)
		const std::filesystem::path project_temp_dll = PROJECT_PATH / "bin" / (PROJECT_NAME + PFF_PROJECT_TEMP_DLL_PATH) / ("lib" + PROJECT_NAME + ".so");
		const std::filesystem::path project_dll = project_dll_path / ("lib" + PROJECT_NAME + ".so");
#endif

// 		if (!std::filesystem::exists(project_temp_dll)) {			// DLL not found => need to recompile

// 			LOG(Trace, "project DLL not found, recompiling")
// #if defined(PFF_PLATFORM_WINDOWS)
// 			const std::filesystem::path path_to_build_script = util::get_executable_path().parent_path() / "PFF_helper" / "PFF_helper.exe";
// #elif defined(PFF_PLATFORM_LINUX)
// 			const std::filesystem::path path_to_build_script = util::get_executable_path().parent_path() / "PFF_helper" / "PFF_helper";
// #endif

// 			std::string cmdArgs = "1 0 0 " + PROJECT_PATH.generic_string();
// 			LOG(Info, "CMD Args: " << cmdArgs.c_str());
// 			VALIDATE(util::run_program(path_to_build_script, cmdArgs), , "project files generated succesfully", "Failed to generate project files")

// 			const auto gmake_command = "python3";
// 			std::string compile_args = (PROJECT_PATH / "setup.py").generic_string() + " --compile";
// 			LOG(Fatal, "calling [" << gmake_command << "] with [" << compile_args << "]")
// 			std::string output;
// 			const bool compile_result = util::run_program(gmake_command, compile_args, false, false, true, &output);
// 			VALIDATE(compile_result, , "project files generated succesfully", "Failed to generate project files [" << output << "]")
// 		}

		VALIDATE(io::copy_file(project_temp_dll, project_dll_path), return, "copyed project DLL from staging dir", "Failed to copy new project DLL");			// copy the new DLL
		VALIDATE(io::is_file(project_dll), return, "", "provided path is not a file [" << project_dll << "]");
		VALIDATE(load_library(project_dll), return, "loaded project DLL", "could not load project [" << project_dll << "]");

		m_add_component_from_string			= (add_component_Fn)try_load_function("add_component");
		m_init_scripts						= (init_scripts_Fn)try_load_function("init_scripts");
		m_serialize_script					= (serialize_script_Fn)try_load_function("serialize_script");
		m_get_scripts						= (get_scripts_Fn)try_load_function("get_all_scripts");
		m_get_procedural_mesh_scripts		= (get_scripts_Fn)try_load_function("get_all_procedural_mesh_scripts");
		m_display_properties				= (display_properties_Fn)try_load_function("display_properties");
		m_is_loaded = true;
	}

	void reload(bool delete_script_components) {

		LOG(Debug, "called script_system::init(delete_script_components=" << util::to_string(delete_script_components) << ")");
		VALIDATE(free_script_library(delete_script_components), return, "", "Count not free scripting library");	
		auto& registry = GET_REGISTRY_OF_MAP;
		LOG(Info, "Registry address in init: " << &registry);
		init();															// overrides func pointers
		reinit_scripts();
	}

	bool free_script_library(bool delete_script_components) {

		// TODO: Add way to clear a pool so that we can remove hot reload only script components, while leaving the rest of the scene intact. This might not be possible, so we shal see 

		if (!m_is_loaded) {

			LOG(Debug, "Scripting Library not loaded yet")
			return true;
		}

		serialize_scripts(serializer::option::save_to_file);
		GET_REGISTRY_OF_MAP.view<procedural_mesh_component>().each([=](auto entity, auto& script_comp) {

			if (script_comp.instance)
				script_comp.destroy_script(&script_comp);
		});
		GET_REGISTRY_OF_MAP.clear<procedural_mesh_component>();
		LOG(Debug, "clearing all [procedural_mesh_component] from registry");
		LOG(Debug, "Number of procedural_mesh_components after clearing: " << GET_REGISTRY_OF_MAP.view<procedural_mesh_component>().size());

		//if (delete_script_components && m_delete_scripts)
		//	m_delete_scripts();

		m_add_component_from_string		= add_component_stub;
		m_init_scripts					= nullptr;
		m_get_scripts					= nullptr;
		m_get_procedural_mesh_scripts	= nullptr;
		m_display_properties			= nullptr;
		m_serialize_script				= nullptr;

		VALIDATE(free_library(), return false, "unloaded project DLL", "could not unload project_DLL");

		m_is_loaded = false;
		m_is_initalized = false;
		LOG(Debug, "freeed scripting library")
		return true;
	}

}
