
VULKAN_SDK = os.getenv("VULKAN_SDK")

glslc = "../PFF/vendor/vulkan-glslc/glslc.exe"

------------------
vendor_path = {}
vendor_path["glfw"]              = "PFF/vendor/glfw"
vendor_path["glm"]               = "PFF/vendor/glm"
vendor_path["ImGui"]             = "PFF/vendor/imgui"
vendor_path["fastgltf"]          = "PFF/vendor/fastgltf/include"
vendor_path["tinyobjloader"]     = "PFF/vendor/tinyobjloader"
vendor_path["stb_image"]         = "PFF/vendor/stb_image"
vendor_path["entt"]              = "PFF/vendor/entt/include"
vendor_path["ImGuizmo"]          = "PFF/vendor/ImGuizmo"


------------ include ------------ 
IncludeDir = {}
IncludeDir["glfw"]              = "%{wks.location}/%{vendor_path.glfw}"
IncludeDir["glm"]               = "%{wks.location}/%{vendor_path.glm}"
IncludeDir["ImGui"]             = "%{wks.location}/%{vendor_path.ImGui}"
IncludeDir["fastgltf"]          = "%{wks.location}/%{vendor_path.fastgltf}"
IncludeDir["tinyobjloader"]     = "%{wks.location}/%{vendor_path.tinyobjloader}"
IncludeDir["stb_image"]         = "%{wks.location}/%{vendor_path.stb_image}"
IncludeDir["entt"]              = "%{wks.location}/%{vendor_path.entt}"
IncludeDir["ImGuizmo"]          = "%{wks.location}/%{vendor_path.ImGuizmo}"

IncludeDir["VulkanSDK"]         = "%{VULKAN_SDK}/include"

------------ libs dir ------------ 
LibraryDir = {}
LibraryDir["VulkanSDK"]         = "%{VULKAN_SDK}/Lib"

------------ libs ------------ 
Library = {}
Library["Vulkan"]               = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"]          = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
