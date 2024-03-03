
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}

IncludeDir["glfw"]              = "%{wks.location}/PFF/vendor/glfw"
IncludeDir["glm"]               = "%{wks.location}/PFF/vendor/glm"
IncludeDir["ImGui"]             = "%{wks.location}/PFF/vendor/imgui_new"
IncludeDir["tinyobjloader"]     = "%{wks.location}/PFF/vendor/tinyobjloader"
IncludeDir["stb_image"]         = "%{wks.location}/PFF/vendor/stb_image"
IncludeDir["VulkanSDK"]         = "%{VULKAN_SDK}/Include"
IncludeDir["yaml_cpp"]          = "%{wks.location}/PFF/vendor/yaml-cpp/include"


LibraryDir = {}

LibraryDir["VulkanSDK"]         = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"]               = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"]          = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
