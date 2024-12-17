
#include "util/pffpch.h"

#include "path_manipulation.h"


namespace PFF::util {


    bool util::is_valid_project_dir(const std::filesystem::path& path) {

        if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
            return false;

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == PFF_PROJECT_EXTENTION)
                return true; // found a project file
        }

        return false;
    }


    std::filesystem::path extract_path_from_project_folder(const std::filesystem::path& full_path) {

        std::string full_path_str = full_path.string();
        std::string folder_marker = std::string(CONTENT_DIR);
        size_t pos = full_path_str.find(folder_marker);
        if (pos != std::string::npos) {

            std::string result_str = full_path_str.substr(pos);
            return std::filesystem::path(result_str);

        } else {

            CORE_LOG(Trace, "NOT FOUND");
            return {};
        }
    }


    std::filesystem::path extract_path_from_project_content_folder(const std::filesystem::path& full_path) {

        std::filesystem::path result;
        bool start_adding = false;

        for (const auto& part : full_path) {

            if (start_adding)
                result /= part;  // Add the part to the result path

            if (part == std::string(CONTENT_DIR))
                start_adding = true;
        }
        return result;
    }


    std::filesystem::path extract_path_from_directory(const std::filesystem::path& full_path, const std::string& directory) {

        std::filesystem::path result;
        bool start_adding = false;

        for (const auto& part : full_path) {

            if (start_adding)
                result /= part;  // Add the part to the result path

            if (part == directory)
                start_adding = true;
        }
        return result;
    }

}
