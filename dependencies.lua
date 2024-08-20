
VULKAN_SDK = os.getenv("VULKAN_SDK")

glslc = "../PFF/vendor/vulkan-glslc/glslc.exe"

------------ include ------------ 
IncludeDir = {}
IncludeDir["glfw"]              = "%{wks.location}/PFF/vendor/glfw"
IncludeDir["glm"]               = "%{wks.location}/PFF/vendor/glm"
IncludeDir["ImGui"]             = "%{wks.location}/PFF/vendor/imgui"
IncludeDir["fastgltf"]          = "%{wks.location}/PFF/vendor/fastgltf/include"
IncludeDir["tinyobjloader"]     = "%{wks.location}/PFF/vendor/tinyobjloader"
IncludeDir["stb_image"]         = "%{wks.location}/PFF/vendor/stb_image"
IncludeDir["entt"]              = "%{wks.location}/PFF/vendor/entt/include"
IncludeDir["ImGuizmo"]          = "%{wks.location}/PFF/vendor/ImGuizmo"

IncludeDir["VulkanSDK"]         = "%{VULKAN_SDK}/Include"

------------ libs dir ------------ 
LibraryDir = {}
LibraryDir["VulkanSDK"]         = "%{VULKAN_SDK}/Lib"

------------ libs ------------ 
Library = {}
Library["Vulkan"]               = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"]          = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
