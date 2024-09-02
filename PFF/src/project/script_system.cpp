
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

#define GET_WORLD_LAYER															application::get().get_world_layer()
#define GET_REGISTRY_OF_MAP														GET_WORLD_LAYER->get_map()->get_registry()
#define TRY_EXECUTE_FUNCTION(name)												if (name) name

	static editor_update_script_Fn												m_editor_update_scripts = nullptr;
	static bool																	m_is_loaded = false;
	static add_component_Fn														m_add_component_from_string = nullptr;
	static init_scripts_Fn														m_init_scripts = nullptr;
	//static ImGui_Fn															m_ImGui = nullptr;

	static void add_component_stub(std::string, PFF::entity)					{ CORE_LOG(Warn, "Adding component from STUB"); }
	static void init_scripts_stub(entt::registry*)								{}
	static void editor_update_script_stub(entt::registry&, float)				{}
	static void delete_scripts_stub()											{}

	void init()																	{ reload(); }
	
	void shutdown()																{ free_script_library(true); }
	void editor_update(float delta_time)										{ TRY_EXECUTE_FUNCTION(m_editor_update_scripts)(GET_REGISTRY_OF_MAP, delta_time); }
	void add_component_from_string(std::string class_name, PFF::entity entity)	{ TRY_EXECUTE_FUNCTION(m_add_component_from_string)(class_name, entity); }
	//void imGui(entity entity)													{ TRY_EXECUTE_FUNCTION(m_ImGui)(entity); }

#ifdef PFF_PLATFORM_WINDOWS
	static HMODULE m_module;
	static FARPROC __stdcall try_load_function(HMODULE module, const char* functionName) {

		auto func = GetProcAddress(module, functionName);
		CORE_VALIDATE(func, , "successfully loaded function [" << functionName << "]", "Could not load dll function [" << functionName << "]")
			return func;
	}
#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
#error Not implemented yet
#endif


	void reinit_scripts() { 

		if (!m_is_loaded || !m_init_scripts)
			return;

		CORE_VALIDATE(application::get().get_world_layer()->get_map(), return, "reinit_scripts", "COULD NOT reinit_scripts");
		m_init_scripts( &GET_REGISTRY_OF_MAP );
	}

	void reload(bool delete_script_components) {

		if (m_is_loaded) {

			if (!free_script_library(delete_script_components))
				return;
		}
		
		const std::filesystem::path project_dll = PROJECT_PATH / "bin" / PROJECT_NAME / (std::string(PROJECT_NAME) + ".dll");
		CORE_VALIDATE(io_handler::is_file(project_dll), return, "", "provided path is not a file [" << project_dll << "]");

#ifdef PFF_PLATFORM_WINDOWS

		m_module = LoadLibraryA(project_dll.string().c_str());
		CORE_VALIDATE(m_module, return, "loaded project DLL", "could not load project [" << project_dll << "]");

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
	#error Not implemented yet
#endif

		m_add_component_from_string		= (add_component_Fn)try_load_function(m_module, "add_component");
		m_init_scripts					= (init_scripts_Fn)try_load_function(m_module, "init_scripts");
		//m_delete_scripts				= (delete_scripts_Fn)try_load_function(m_module, "delete_scripts");
		m_is_loaded						= true;

		reinit_scripts();
	}


	bool free_script_library(bool delete_script_components) {

		// TODO: Add way to clear a pool so that we can remove hot reload only script components, while leaving the rest of the scene intact. This might not be possible, so we shal see 
		if (!m_is_loaded)
			return true;

		//if (delete_script_components && m_delete_scripts)
		//	m_delete_scripts();

		m_add_component_from_string = add_component_stub;
		m_init_scripts = init_scripts_stub;
		//m_editor_update_scripts = editor_update_script_stub;
		//m_serialize_scripts = serialize_scripts_stub;
		//m_update_scripts = update_script_stub;
		//m_init_ImGui = init_ImGui_stub;
		//m_ImGui = ImGui_stub;
		//m_delete_scripts = delete_scripts_stub;
		//m_notify_begin_contact = notify_begin_contact_stub;
		//m_notify_end_contact = notify_end_contact_stub;

#ifdef PFF_PLATFORM_WINDOWS
#define FREE_LIBRARY();																		
	if (!FreeLibrary(m_module)) {															
		DWORD errorCode = GetLastError();													
		CORE_LOG(Warn, "Could not free script dll. Error Code: " << errorCode);				
		return false;																		
	}
#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
#define FREE_LIBRARY();		#error Not implemented yet
#endif

		m_module = nullptr;
		m_is_loaded = false;
		return true;
	}

}
