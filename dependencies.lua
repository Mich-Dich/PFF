
VULKAN_SDK = os.getenv("VULKAN_SDK")

------------ inlude ------------ 
IncludeDir = {}
IncludeDir["glfw"]              = "%{wks.location}/PFF/vendor/glfw"
IncludeDir["glm"]               = "%{wks.location}/PFF/vendor/glm"
IncludeDir["ImGui"]             = "%{wks.location}/PFF/vendor/imgui"
IncludeDir["tinyobjloader"]     = "%{wks.location}/PFF/vendor/tinyobjloader"
IncludeDir["stb_image"]         = "%{wks.location}/PFF/vendor/stb_image"
IncludeDir["VulkanSDK"]         = "%{VULKAN_SDK}/Include"

------------ libs dir ------------ 
LibraryDir = {}
LibraryDir["VulkanSDK"]         = "%{VULKAN_SDK}/Lib"

------------ libs ------------ 
Library = {}
Library["Vulkan"]               = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"]          = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
