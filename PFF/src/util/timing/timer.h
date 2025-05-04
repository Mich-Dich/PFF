#pragma once

#include "util/pffpch.h"

// class PFF::application;

namespace PFF::util {

    // struct timer {
    
    //     std::future<void> future;
    //     std::shared_ptr<std::pair<std::atomic<bool>, std::condition_variable>> shared_state;
    // };
    
    // // @brief Asynchronously starts a timer with the specified duration and callback function.
    // // 
    // // @brief Usage example:
    // // @brief     util::timer_async(std::chrono::seconds(2), []() { LOG(Info, "Timer Done");  });
    // // @brief Or:
    // // @brief     auto timer = util::timer_async(std::chrono::seconds(2), []() { LOG(Info, "Timer Done");  });
    // // 
    // // @param duration The duration of the timer.
    // // @param callback The callback function to be executed when the timer finishes.
    // // @return Reference to the std::future<void> associated with the timer.
    // template<class _rep, class _period>
    // std::future<void>& timer_async(std::chrono::duration<_rep, _period> duration, std::function<void()> callback, std::function<void()> cancle_callback = NULL) {
    
    //     auto shared_state = std::make_shared<std::pair<std::atomic<bool>, std::condition_variable>>();
    //     shared_state->first = false;
    //     auto future = std::async(std::launch::async, [duration, callback, cancle_callback, shared_state]() {
    
    //         std::mutex mtx;
    //         std::unique_lock<std::mutex> lock(mtx);
    
    //         if (shared_state->second.wait_for(lock, duration, [shared_state]() { return shared_state->first.load(); })) {
    //             if(cancle_callback != NULL)             // Woken up by main thread
    //                 cancle_callback();
    //         } else
    //             callback();                             // Time expired
    
    //         //application::get().remove_timer(future);
    //     });
    
    //     auto loc_app = PFF::application::get();
    //     return loc_app.add_future(future, shared_state);
    // }
    
    
    // void cancel_timer(timer& timer);
    
    // // @brief Checks if the specified timer has finished.
    // //
    // // @brief  Usage example:
    // // @brief      if (is_timer_ready(timer, 1ms)) { // do something }
    // // 
    // // @param timer The timer to check.
    // // @param duration The duration to wait before checking the timer status. Default is 1ms.
    // // @return True if the timer has finished, false otherwise.
    // template<class _rep, class _period>
    // bool is_timer_ready(std::future<void>& timer, std::chrono::duration<_rep, _period> duration = std::chrono::milliseconds(1)) { return timer.wait_for(duration) == std::future_status::ready; }

}
