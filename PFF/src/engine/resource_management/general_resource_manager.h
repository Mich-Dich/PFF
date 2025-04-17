#pragma once

#include "engine/resource_management/headers.h"

namespace PFF::resource_manger {

	enum class asset_curruption_source {
		unknown = 0,
		empty_file,
		header_incorrect,
	};
    
	bool try_to_deserialize_file_header(const std::filesystem::path& file_path, const bool log_messages, asset_curruption_source& asset_curruption_source, asset_file_header& loc_asset_file_header);

}
