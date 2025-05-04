
#include "util/pch_editor.h"
#include "ui/import/mesh_import_window.h"
#include "PFF_editor.h"

#include "import_dialog.h"



namespace PFF::util {

	// std::thread import_dialog_thread;


	// void import_dialog_impl(const import_type type, const std::filesystem::path destination_path) {
		
	// 	switch (type) {
	// 		case import_type::mesh: {

	// 			const std::vector<std::pair<std::string, std::string>> import_file_types = {
	// 				{"glTF 2.0 file", "*.gltf;*.glb"},
	// 			};
	// 			std::filesystem::path source_path = util::file_dialog("Import asset", import_file_types);
	// 			if (source_path.extension() == ".gltf" || source_path.extension() == ".glb")
	// 				PFF_editor::get().get_editor_layer()->add_window<mesh_import_window>(source_path, destination_path);

	// 		} break;
	// 		case import_type::texture: {
	
	// 		} break;
		
	// 		default: LOG(Error, "Tryed to import a file but provided asset type is unknown [" << type << "]"); break;
	// 	}
	// }

	// void open_import_dialog(const import_type type, const std::filesystem::path destination_path) {

	// 	import_dialog_thread = std::thread(import_dialog_impl, type, destination_path);

	// }

}