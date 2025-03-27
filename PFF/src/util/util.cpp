
#include <util/pffpch.h>

#include "util.h"

namespace PFF::util {

    void init() {

    #if defined(PFF_PLATFORM_LINUX)
        init_qt();
    #endif

    }

    void shutdown() {

    #if defined(PFF_PLATFORM_LINUX)
        shutdown_qt();
    #endif

    }

}
