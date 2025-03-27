
#include "util/pffpch.h"

#include "simple_deletion_queue.h"

namespace PFF::util {


    void simple_deletion_queue::flush() {

        for (auto it = m_functions.rbegin(); it != m_functions.rend(); it++)
            (*it)();
        m_functions.clear();
    }

}
