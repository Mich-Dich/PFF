project(PFF_editor)

file(GLOB_RECURSE EDITOR_SOURCES "src/*.cpp" "src/*.h")
add_executable(PFF_editor ${EDITOR_SOURCES})

# Ensure PFF is built before PFF_editor
add_dependencies(PFF_editor PFF)

target_include_directories(PFF_editor PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../PFF/src"
    "${ImGui_DIR}"
    "${VulkanSDK_DIR}/Include"
)

target_link_libraries(PFF_editor
    PFF
    imgui
)

target_compile_definitions(PFF_editor PRIVATE
    PFF_PLATFORM_WINDOWS
    PFF_INSIDE_EDITOR
    ENGINE_NAME="PFF_editor"
)

set_target_properties(PFF_editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin
)

