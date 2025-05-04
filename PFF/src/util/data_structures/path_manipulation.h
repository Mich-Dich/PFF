#pragma once


namespace PFF::util {

    bool is_valid_project_dir(const std::filesystem::path& path);

    std::filesystem::path extract_path_from_project_folder(const std::filesystem::path& full_path);

    std::filesystem::path extract_path_from_project_content_folder(const std::filesystem::path& full_path);

    std::filesystem::path extract_path_from_directory(const std::filesystem::path& full_path, const std::string& directory);

}
