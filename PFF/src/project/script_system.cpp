
#include "util/pffpch.h"

#include "application.h"
#include "engine/world/map.h"
#include "engine/layer/world_layer.h"

#ifdef PFF_PLATFORM_WINDOWS
	#include <Windows.h>
#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
	#error Not implemented yet
#endif

#include "script_system.h"

namespace PFF::script_system {

	static add_component_Fn			m_add_component_from_string = nullptr;
	static update_script_Fn			m_update_scripts = nullptr;
	static editor_update_script_Fn	m_editor_update_scripts = nullptr;
	static serialize_scripts_Fn		m_serialize_scripts = nullptr;
	static init_scripts_Fn			m_init_scripts = nullptr;
	static init_ImGui_Fn			m_init_ImGui = nullptr;
	static ImGui_Fn					m_ImGui = nullptr;
	static delete_scripts_Fn		m_delete_scripts = nullptr;
	static notify_begin_contact_Fn	m_notify_begin_contact = nullptr;
	static notify_end_contact_Fn	m_notify_end_contact = nullptr;
	static bool						m_is_loaded = false;

#define GET_WORLD_LAYER					application::get().get_world_layer()
#define GET_REGISTRY_OF_MAP				GET_WORLD_LAYER->get_maps()[0].get()->get_registry()
#define TRY_EXECUTE_FUNCTION(name)		if (name) name

	static void add_component_stub(std::string, entt::entity) { CORE_LOG(Warn, "Adding component from STUB"); }
	static void update_script_stub(entt::registry&, float) {}
	static void editor_update_script_stub(entt::registry&, float) {}
	static void serialize_scripts_stub(serializer::option, const std::filesystem::path&, PFF::entity) {}
	static void init_scripts_stub(world_layer*) {}
	static void init_ImGui_stub(void*) {}
	static void ImGui_stub(entity) {}
	static void delete_scripts_stub() {}
	static void notify_begin_contact_stub(entity a, entity b) {}
	static void notify_end_contact_stub(entity a, entity b) {}

#ifdef PFF_PLATFORM_WINDOWS
	
	static HMODULE m_module;

	static FARPROC __stdcall try_load_function(HMODULE module, const char* functionName) {

		auto func = GetProcAddress(module, functionName);
		if (func == NULL) 
			CORE_LOG(Warn, "Could not load dll function [" << functionName << "]");

		return func;
	}

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
	
	#error Not implemented yet

#endif

	void init()				{ reload(); }
	void shutdown()			{ free_script_library(true); }

	void update(float delta_time)																		{ TRY_EXECUTE_FUNCTION(m_update_scripts)(GET_REGISTRY_OF_MAP, delta_time); }
	void editor_update(float delta_time)																{ TRY_EXECUTE_FUNCTION(m_editor_update_scripts)(GET_REGISTRY_OF_MAP, delta_time); }
	void notify_begin_contact(entity entityA, entity entityB)											{ TRY_EXECUTE_FUNCTION(m_notify_begin_contact)(entityA, entityB); }
	void notify_end_contact(entity entityA, entity entityB)												{ TRY_EXECUTE_FUNCTION(m_notify_end_contact)(entityA, entityB); }
	void imGui(entity entity)																			{ TRY_EXECUTE_FUNCTION(m_ImGui)(entity); }
	void init_ImGui(void* context)																		{ TRY_EXECUTE_FUNCTION(m_init_ImGui)(context); }
	void serialize(const serializer::option option, const std::filesystem::path& path, entity entity)	{ TRY_EXECUTE_FUNCTION(m_serialize_scripts)(option, path, entity); }
	void add_component_from_string(std::string class_name, entt::entity entity)							{ TRY_EXECUTE_FUNCTION(m_add_component_from_string)(class_name, entity); }


#define DEV_ONLY
#ifdef DEV_ONLY

	typedef void (*test_func)();
	static test_func		m_test_func = nullptr;

#endif

	void reload(bool delete_script_components) {

		if (m_is_loaded) {

			if (!free_script_library(delete_script_components))
				return;
		}
		


		const std::filesystem::path project_dll = PROJECT_PATH / "bin" / PROJECT_NAME / (std::string(PROJECT_NAME) + ".dll");
		CORE_VALIDATE(io_handler::is_file(project_dll), return, "", "provided path is not a file [" << project_dll << "]");

		m_module = LoadLibraryA(project_dll.string().c_str());
		CORE_VALIDATE(m_module, return, "loaded project DLL", "could not load project [" << project_dll << "]");
		
		CORE_LOG(Trace, "try to load a test function")
		m_test_func = (test_func)try_load_function(m_module, "test_function");

		CORE_LOG(Trace, "test function");
		TRY_EXECUTE_FUNCTION(m_test_func)();



		m_serialize_scripts			= (serialize_scripts_Fn)try_load_function(m_module, "save_scripts");
		m_update_scripts			= (update_script_Fn)try_load_function(m_module, "update_scripts");
		m_editor_update_scripts		= (update_script_Fn)try_load_function(m_module, "editor_update_scripts");
		m_add_component_from_string	= (add_component_Fn)try_load_function(m_module, "add_component");
		m_init_scripts				= (init_scripts_Fn)try_load_function(m_module, "init_scripts");
		m_init_ImGui				= (init_ImGui_Fn)try_load_function(m_module, "init_ImGui");
		m_ImGui						= (ImGui_Fn)try_load_function(m_module, "ImGui");
		m_delete_scripts			= (delete_scripts_Fn)try_load_function(m_module, "delete_scripts");
		m_notify_begin_contact		= (notify_begin_contact_Fn)try_load_function(m_module, "notify_begin_contact");
		m_notify_end_contact		= (notify_end_contact_Fn)try_load_function(m_module, "notify_end_contact");
		m_is_loaded					= true;

		if (m_init_scripts)
			m_init_scripts(application::get().get_world_layer());
	}

	bool free_script_library(bool delete_script_components) {

		// TODO: Add way to clear a pool so that we can remove hot reload only script components, while leaving the rest of the scene intact. This might not be possible, so we shal see 
		if (!m_is_loaded)
			return true;

		if (delete_script_components && m_delete_scripts)
			m_delete_scripts();

		m_serialize_scripts = serialize_scripts_stub;
		m_update_scripts = update_script_stub;
		m_editor_update_scripts = editor_update_script_stub;
		m_add_component_from_string = add_component_stub;
		m_init_scripts = init_scripts_stub;
		m_init_ImGui = init_ImGui_stub;
		m_ImGui = ImGui_stub;
		m_delete_scripts = delete_scripts_stub;
		m_notify_begin_contact = notify_begin_contact_stub;
		m_notify_end_contact = notify_end_contact_stub;

		if (!FreeLibrary(m_module)) {

#ifdef PFF_PLATFORM_WINDOWS
			DWORD errorCode = GetLastError();
#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
	#error Not implemented yet
#endif
			CORE_LOG(Warn, "Could not free script dll. Error Code: " << errorCode);
			return false;
		}

		m_module = nullptr;
		m_is_loaded = false;
		return true;
	}

}
