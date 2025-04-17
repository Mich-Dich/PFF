
#include "util/pffpch.h"

#include "util/io/serializer_binary.h"

#include "general_resource_manager.h"


namespace PFF::resource_manager {

    
	bool try_to_deserialize_file_header(const std::filesystem::path& file_path, const bool log_messages, asset_curruption_source& asset_curruption_source, asset_file_header& loc_asset_file_header) {

		bool file_currupted = false;
		asset_curruption_source = asset_curruption_source::unknown;

		std::error_code loc_error_code;
		if (!std::filesystem::file_size(file_path, loc_error_code)) {

			asset_curruption_source = asset_curruption_source::empty_file;
			file_currupted = true;

			std::string faulty_file_name = file_path.filename().generic_string();
			if (log_messages)
				LOG(Warn, "file [" << faulty_file_name << "] is an empty file, will be marked as currupted");
		}


		if (file_path.extension() == ".pffasset") {

			serializer::binary(file_path, "PFF_asset_file", serializer::option::load_from_file)
				.entry(loc_asset_file_header);

		} else if (file_path.extension() == ".pffworld") {

			serializer::yaml(file_path, "map_data", serializer::option::load_from_file)
				.sub_section("file_header", [&](serializer::yaml& header_section) {

					header_section.entry(KEY_VALUE(loc_asset_file_header.file_version))
					.entry(KEY_VALUE(loc_asset_file_header.type))
					.entry(KEY_VALUE(loc_asset_file_header.timestamp));
			});
		}

		return file_currupted;
	}


}
