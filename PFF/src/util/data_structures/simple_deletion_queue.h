#pragma once

namespace PFF::util {

    // @brief A class that manages a queue of functions for deferred execution. 
    //        Typically used for managing cleanup operations that should be executed 
    //        at a later time, such as resource deallocation.
    class simple_deletion_queue {
    public:

        // @brief Adds a function to the deletion queue.
       // @param [function] A function object (std::function<void()>) to be added to the queue.
       //                   The function is stored and executed later when flush() is called.
        void push_func(std::function<void()>&& function) { m_functions.push_back(function); }

        // @brief Executes all the functions in the queue and clears the queue.
        //        Each function in the queue is called in the order it was added.
        //        After all functions are executed, the queue is emptied.
        void flush();

    private:
        std::vector<std::function<void()>>		m_functions{};
    };

}
