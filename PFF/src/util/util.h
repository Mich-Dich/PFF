#pragma once

// =================================================================================  includes  ==================================================================================

#include <type_traits> // For std::is_vector

#include <typeinfo>

#if defined(__GNUC__) || defined(__clang__)
    #include <cxxabi.h>
#endif

#include "application.h"

#include "util/data_types.h"
#include "util/macros.h"

// ==================================================================================  utils  ==================================================================================
#include "util/system.h"

// 
#include "util/data_structures/path_manipulation.h"
#include "util/data_structures/simple_deletion_queue.h"
#include "util/data_structures/string_manipulation.h"
#include "util/data_structures/UUID.h"

#include "util/io/config.h"
#include "util/io/io.h"
#include "util/io/logger.h"
#include "util/io/serializer_binary.h"
#include "util/io/serializer_data.h"
#include "util/io/serializer_yaml.h"

#include "util/math/math.h"
#include "util/math/constance.h"
#include "util/math/noise.h"
#include "util/math/random.h"

#include "util/timing/instrumentor.h"
#include "util/timing/stopwatch.h"
#include "util/timing/timer.h"

#include "util/ui/imgui_markdown.h"
#include "util/ui/pannel_collection.h"

#if defined(PFF_PLATFORM_WINDOWS)
    #include "util/platform/windows_util.h"
#elif defined(PFF_PLATFORM_LINUX)
    #include "util/platform/linux_util.h"
#elif defined(PFF_PLATFORM_MAC)
    #include "util/platform/mac_util.h"
#endif





namespace PFF::util {

    void init();
    void shutdown();

    // ================================================= utilits =================================================

    template<typename key_type, typename value_type>
    bool contains(const std::unordered_map<key_type, value_type>& map, const key_type& key) { return map.find(key) != map.end(); }

    //@brief Checks the validity of a pointer.
    //@param [ptr] Pointer to check.
    //@return [conat char*] "valid" if the pointer is not null, "invalid" otherwise.
    FORCEINLINE constexpr const char* ptr_validity_check(const void* ptr) { return (ptr != nullptr) ? "valid" : "invalid"; }
}
