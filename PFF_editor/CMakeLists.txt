project(PFF_editor)

file(GLOB_RECURSE EDITOR_SOURCES "src/*.cpp" "src/*.h")
add_executable(PFF_editor ${EDITOR_SOURCES})

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
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/../bin
)

add_custom_command(TARGET PFF_editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/../bin/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/logo.png
        ${CMAKE_BINARY_DIR}/../bin/assets/logo.png
)
