#pragma once

#include "engine/render/image.h"


namespace PFF {

    class icon_manager {
    public:

        icon_manager();
        ~icon_manager();
        PFF_DELETE_COPY_MOVE_CONSTRUCTOR(icon_manager);

        ref<image> request_icon(const std::filesystem::path& asset_path);

		// void release_all();
        
    private:

        void worker_loop();
        void create_icon(const std::filesystem::path& path);

        // static icon_manager                                     s_instamce{};

        std::unordered_map<size_t, ref<image>>                  m_cache{};
        std::mutex                                              m_mutex{};
        
        // Job queue + helper set to enforce uniqueness
        std::deque<std::filesystem::path>                       m_job_queue{};
        std::unordered_set<size_t>                              m_jobs_in_queue{};

        std::condition_variable                                 m_conditional_var{};
        bool                                                    m_shutting_down = false;
        std::thread                                             m_worker{};
    };
    
}
