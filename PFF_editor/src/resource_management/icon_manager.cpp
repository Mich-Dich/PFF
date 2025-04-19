
#include "util/pch_editor.h"

#include <engine/resource_management/headers.h>
#include <engine/resource_management/general_resource_manager.h>
#include <engine/resource_management/texture_headers.h>
#include <engine/resource_management/texture_serializer.h>
#include <engine/resource_management/image_asset_manager.h>

#include "icon_manager.h"

namespace PFF {
    
    icon_manager::icon_manager() {

        m_worker = std::thread(&icon_manager::worker_loop, this);
    }
    
    icon_manager::~icon_manager() {
    
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            m_shutting_down = true;
        }
        m_conditional_var.notify_all();
        if (m_worker.joinable())
            m_worker.join();
    }
    

    ref<image> icon_manager::request_icon(const std::filesystem::path& asset_path) {

        const size_t path_hash = std::filesystem::hash_value(asset_path);
        const auto it = m_cache.find(path_hash);
        if (it != m_cache.end())
            return it->second;

        // If not already queued, enqueue
        if (m_jobs_in_queue.insert(path_hash).second) {
            m_job_queue.push_back(asset_path);
            m_conditional_var.notify_one();
        }

        return nullptr;
    }


    void icon_manager::worker_loop() {

        while (true) {
            std::filesystem::path job_path;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_conditional_var.wait(lock, [this]{
                    return m_shutting_down || !m_job_queue.empty();
                });
    
                if (m_shutting_down && m_job_queue.empty())
                    return;
    
                job_path = std::move(m_job_queue.front());
                m_job_queue.pop_front();
                m_jobs_in_queue.erase(std::filesystem::hash_value(job_path));
            }
    
            create_icon(job_path);                  // Generate the icon (blocking I/O + processing)
        }
    }


    // TODO: make faster        maybe SIMD
	FORCEINLINE void downscale_image(const u32* pixel_data, u32 width, u32 height, u32 target_size, std::vector<u32>& out_pixels, u32 &new_width, u32 &new_height) {

		VALIDATE(pixel_data != nullptr, return, "", "didnt provide pixel data, aborting function")

		u32 maxSide = (width > height) ? width : height;
		f32 scale = static_cast<f32>(target_size) / maxSide;

		new_width = static_cast<u32>(width * scale);
		new_height = static_cast<u32>(height * scale);
		out_pixels.resize(new_width * new_height);

		for (u32 y = 0; y < new_height; y++) {
			for (u32 x = 0; x < new_width; x++) {

				// Map the coordinates in the new image back to the original image.
				// Using nearest neighbor by simply converting floating point coordinates
				// to the nearest integer coordinate.
				u32 orig_x = static_cast<u32>(x / scale);
				u32 orig_y = static_cast<u32>(y / scale);

				// Clamp to the bounds of the original image.
				if (orig_x >= width)  orig_x = width - 1;
				if (orig_y >= height) orig_y = height - 1;
				
				out_pixels[y * new_width + x] = pixel_data[orig_y * width + orig_x];
			}
		}
	}


    void icon_manager::create_icon(const std::filesystem::path& path) {

		size_t hash_value = std::filesystem::hash_value(path);
		resource_manager::asset_curruption_source loc_file_curruption_source = resource_manager::asset_curruption_source::unknown;
		asset_file_header loc_asset_file_header;
		bool file_deserialized = resource_manager::try_to_deserialize_file_header(path, true, loc_file_curruption_source, loc_asset_file_header);
        
        ref<image> loc_image_ref = nullptr;
        switch (loc_asset_file_header.type) {
            case file_type::mesh:                   LOG(Warn, "icon creation for mesh not implemented yet") break;
            case file_type::world:                  LOG(Warn, "icon creation for world not implemented yet") break;
            case file_type::material:               LOG(Warn, "icon creation for material not implemented yet") break;
            case file_type::material_instance:      LOG(Warn, "icon creation for material_instance not implemented yet") break;
            case file_type::texture: {

                    u32* pixel_data;
                    asset_file_header loc_asset_header{};
                    general_texture_file_header loc_general_header{};
                    specific_texture_file_header loc_specific_texture_header{};
                    serialize_texture(path, pixel_data, loc_asset_header, loc_general_header, loc_specific_texture_header, serializer::option::load_from_file);
    
                    std::vector<u32> new_pixels;
                    downscale_image(pixel_data, loc_specific_texture_header.width, loc_specific_texture_header.height, 128, new_pixels, loc_specific_texture_header.width, loc_specific_texture_header.height);
                    loc_image_ref = create_ref<image>(new_pixels.data(), loc_specific_texture_header.width, loc_specific_texture_header.height, image_format::RGBA);
    
                    new_pixels.clear();
                    delete pixel_data;

            } break;

            default:    LOG(Warn, "provided asset path is invallid") break;
        }

        if (loc_image_ref) {                                                    // if everything worked add icon

            std::lock_guard<std::mutex> lock(m_mutex);
            m_cache[hash_value] = loc_image_ref;
        } else
            LOG(Warn, "Failed to generate icon for [" << path.generic_string() << "]")

    }

}
