project(PFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies.cmake)

add_subdirectory(${GLFW_DIR})
add_subdirectory(${ImGui_DIR})
add_subdirectory(${fastgltf_DIR})

file(GLOB_RECURSE PFF_SOURCES "src/*.cpp" "src/*.h")
add_library(PFF SHARED ${PFF_SOURCES})

target_include_directories(PFF PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${VulkanSDK_DIR}/Include"
    "${STB_IMAGE_DIR}"
    "${ENTT_DIR}"
    "${GLFW_DIR}/include"
)

find_package(Vulkan REQUIRED)

target_link_libraries(PFF
    glfw
    imgui
    fastgltf
    Vulkan::Vulkan
)

target_compile_definitions(PFF PRIVATE
    PFF_USING_EDITOR
    PFF_PLATFORM_WINDOWS
    ENGINE_NAME="PFF"
    PROJECT_NAME="PFF"
    PFF_INSIDE_ENGINE
    GLFW_INCLUDE_NONE
)

set_target_properties(PFF PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/../bin
)

