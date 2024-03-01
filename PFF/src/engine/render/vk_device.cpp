
#include "util/pffpch.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "engine/platform/pff_window.h"

#include "vk_device.h"

namespace PFF {


    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // - - - - - Setup
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // class member functions
    vk_device::vk_device(std::shared_ptr<pff_window>& m_window) : m_window{ m_window } {

        PFF_PROFILE_FUNCTION();

        create_instance();
        setup_debug_messenger();
        create_surface();
        pick_physical_device();
        create_logical_device();
        create_command_pool();
    }

    vk_device::~vk_device() {

        PFF_PROFILE_FUNCTION();

        LOG(Info, "start shutdown");
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        vkDestroyDevice(m_device, nullptr);

        if (c_enable_validation_layers)
            destroy_debug_utils_messengerEXT(m_VkInstance, m_debug_messanger, nullptr);

        vkDestroySurfaceKHR(m_VkInstance, m_surface, nullptr);
        vkDestroyInstance(m_VkInstance, nullptr);

        m_window.reset();
        LOG(Info, "shutdown");
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // - - - - - Debug messenger functions
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //
    void vk_device::setup_debug_messenger() {

        PFF_PROFILE_FUNCTION();

        if (!c_enable_validation_layers) 
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populate_debug_messenger_CI(createInfo);

        CORE_VALIDATE(create_debug_utils_messengerEXT(m_VkInstance, &createInfo, nullptr, &m_debug_messanger) == VK_SUCCESS, return, "Successfully set up debug messanger", "FAILED to set up debug messenger");
    }

    //
    VkResult vk_device::create_debug_utils_messengerEXT( VkInstance m_VkInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

        PFF_PROFILE_FUNCTION();

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(m_VkInstance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    //
    void vk_device::destroy_debug_utils_messengerEXT(VkInstance m_VkInstance, VkDebugUtilsMessengerEXT m_debug_messanger, const VkAllocationCallbacks* pAllocator) {

        PFF_PROFILE_FUNCTION();

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(m_VkInstance, m_debug_messanger, pAllocator);
        }
    }

    //
    void vk_device::populate_debug_messenger_CI(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {

        PFF_PROFILE_FUNCTION();

        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debug_callback;
        createInfo.pUserData = nullptr;
    }

    //
    VKAPI_ATTR VkBool32 VKAPI_CALL vk_device::debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* Callback, void* UserData) {

        PFF_PROFILE_FUNCTION();

        // typedef enum VkDebugUtilsMessageTypeFlagBitsEXT {
        //     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT = 0x00000001,
        //     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT = 0x00000002,
        //     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT = 0x00000004,
        //     VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT = 0x00000008,
        //     VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT = 0x7FFFFFFF
        // } VkDebugUtilsMessageTypeFlagBitsEXT;
        // 
        //		Possible Levels for [messageSeverity]
        //	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT		=> Diagnostic message
        //	VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT		=> Informational message like the creation of a resource
        //	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT		=> Message about behavior that is not necessarily an error, but very likely a bug in your application
        //	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT		=> Message about behavior that is invalid and may cause crashes
        // 
        //		Possible Levels for [messageType]
        //	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT			=> Some event has happened that is unrelated to the specification or performance
        //	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT		=> Something has happened that violates the specification or indicates a possible mistake
        //	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT		=> Potential non-optimal use of Vulkan

        //		Possible Levels for [pCallbaclData]
        //	pMessage											=> The debug message as a null-terminated string
        //	pObjects											=> Array of Vulkan object handles related to the message
        //	objectCount											=> Number of objects in array

        switch (msgSeverity) {

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                CORE_LOG(Trace, "Validation Layer: " << vk_debug_message_type_to_string(msgType) << Callback->pMessage)
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                CORE_LOG(Debug, "Validation Layer: " << vk_debug_message_type_to_string(msgType) << Callback->pMessage)
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                CORE_LOG(Warn, "Validation Layer: " << vk_debug_message_type_to_string(msgType) << Callback->pMessage)
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                CORE_LOG(Error, "Validation Layer: " << vk_debug_message_type_to_string(msgType) << Callback->pMessage)
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                CORE_LOG(Fatal, "Validation Layer: " << vk_debug_message_type_to_string(msgType) << Callback->pMessage)
                break;
            default:
                break;
        }

        return VK_FALSE;
    }



    VkResult create_debug_utils_messengerEXT( VkInstance m_VkInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

        PFF_PROFILE_FUNCTION();

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(m_VkInstance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void destroy_debug_utils_messengerEXT( VkInstance m_VkInstance, VkDebugUtilsMessengerEXT m_debug_messanger, const VkAllocationCallbacks* pAllocator) {

        PFF_PROFILE_FUNCTION();

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(m_VkInstance, m_debug_messanger, pAllocator);
        }
    }


    void vk_device::create_instance() {

        PFF_PROFILE_FUNCTION();

        CORE_ASSERT(!(c_enable_validation_layers && !check_validation_layer_support()),"", "validation layers requested, but not available!");

        u32 version{};
        vkEnumerateInstanceVersion(&version);
        version &= ~(0xFFFU);
        CORE_LOG(Trace, "System supports VULKAN version: " << VK_API_VERSION_VARIANT(version)
            << "." << VK_API_VERSION_MAJOR(version) 
            << "." << VK_API_VERSION_MINOR(version) 
            << "." << VK_API_VERSION_PATCH(version));

        // ============= TEST ==============================
        version = VK_MAKE_API_VERSION(0, 1, 2, 0);
        // ============= TEST ==============================

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "PFF_Engine";
        appInfo.applicationVersion = version;
        appInfo.pEngineName = "PFF Vulkan Engine";
        appInfo.engineVersion = version;
        appInfo.apiVersion = version;

        auto extensions = get_required_extensions();

        VkInstanceCreateInfo instance_CI = {};
        instance_CI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_CI.pApplicationInfo = &appInfo;
        instance_CI.enabledExtensionCount = static_cast<u32>(extensions.size());
        instance_CI.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (c_enable_validation_layers) {

            instance_CI.enabledLayerCount = static_cast<u32>(m_validation_layers.size());
            instance_CI.ppEnabledLayerNames = m_validation_layers.data();
            populate_debug_messenger_CI(debugCreateInfo);
            instance_CI.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

        } else {

            instance_CI.enabledLayerCount = 0;
            instance_CI.pNext = nullptr;
        }

        CORE_ASSERT(vkCreateInstance(&instance_CI, nullptr, &m_VkInstance) == VK_SUCCESS, "", "failed to create m_VkInstance!");
       
        has_required_Instance_extensions(extensions);
    }

    void vk_device::pick_physical_device() {

        PFF_PROFILE_FUNCTION();

        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
        CORE_ASSERT(deviceCount > 0, "", "No GPUs found suporting Vulkan");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

        std::multimap<int, VkPhysicalDevice> candidates;

        CORE_LOG(Info, "Num of GPUs supporting Vulkan: " << deviceCount)
        for (int16 x = 0; x < devices.size(); x++) {

            if (!is_device_suitable(devices[x])) {

                CORE_LOG(Warn, CONSOLE_LIST_BEGIN << "device [" << x << "] is NOT SUTABLE");
                continue;
            }

            int score = rateDeviceSuitability(devices[x]);
            candidates.insert(std::make_pair(score, devices[x]));
            CORE_LOG(Trace, CONSOLE_LIST_BEGIN << "device [" << x << "] has a Score of [" << score << "]");
        }

        if (candidates.rbegin()->first > 0) {
            m_physical_device = candidates.rbegin()->second;
            CORE_LOG(Debug, "Fount Device is sutable");
        } else {
            CORE_LOG(Error, "FAILED to find autable Device");
        }
        /*
        for (const auto& get_device : devices) {
            if (is_device_suitable(get_device)) {
                m_physical_device = get_device;
                break;
            }
        }
        */
        
        CORE_ASSERT(m_physical_device != VK_NULL_HANDLE, "", "failed to find a suitable GPU!");
        vkGetPhysicalDeviceProperties(m_physical_device, &properties);
        LOG(Info, "physical device: " << properties.deviceName);
        LOG(Info, "device type: " << physical_device_type_to_string(properties.deviceType));
    }


    void vk_device::create_logical_device() {

        PFF_PROFILE_FUNCTION();

        vk_util::QueueFamilyIndices indices = find_queue_families(m_physical_device);

        std::vector<VkDeviceQueueCreateInfo> device_queues_CI;
        std::set<u32> unique_queue_families = { indices.graphicsFamily, indices.presentFamily };

        float queue_priority = 1.0f;
        for (u32 queue_family : unique_queue_families) {
            VkDeviceQueueCreateInfo queue_CI = {};
            queue_CI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_CI.queueFamilyIndex = queue_family;
            queue_CI.queueCount = 1;
            queue_CI.pQueuePriorities = &queue_priority;
            device_queues_CI.push_back(queue_CI);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo device_CI = {};
        device_CI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        device_CI.queueCreateInfoCount = static_cast<u32>(device_queues_CI.size());
        device_CI.pQueueCreateInfos = device_queues_CI.data();

        device_CI.pEnabledFeatures = &deviceFeatures;
        device_CI.enabledExtensionCount = static_cast<u32>(m_device_extensions.size());
        device_CI.ppEnabledExtensionNames = m_device_extensions.data();

        // might not really be necessary anymore because device specific validation layers
        // have been deprecated
        if (c_enable_validation_layers) {

            device_CI.enabledLayerCount = static_cast<u32>(m_validation_layers.size());
            device_CI.ppEnabledLayerNames = m_validation_layers.data();
        } else 
            device_CI.enabledLayerCount = 0;
        
        CORE_ASSERT(vkCreateDevice(m_physical_device, &device_CI, nullptr, &m_device) == VK_SUCCESS, "", "failed to create logical get_device!");

        vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, indices.presentFamily, 0, &m_present_queue);
    }


    void vk_device::create_command_pool() {

        PFF_PROFILE_FUNCTION();

        vk_util::QueueFamilyIndices queueFamilyIndices = find_physical_queue_families();

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

        CORE_ASSERT(vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) == VK_SUCCESS, "", "failed to create command pool!");
    }


    void vk_device::create_surface() {
        
        m_window->create_window_surface(m_VkInstance, &m_surface);
    }


    bool vk_device::is_device_suitable(VkPhysicalDevice device) {

        PFF_PROFILE_FUNCTION();

        vk_util::QueueFamilyIndices indices = find_queue_families(device);
        bool extensionsSupported = check_device_extension_support(device);
        bool swapChainAdequate = false;
        if (extensionsSupported) {

            SwapChainSupportDetails swapChainSupport = query_swapchain_support(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty() && swapChainSupport.capabilities.minImageCount >= 2;
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate &&
            supportedFeatures.samplerAnisotropy;
    }


    int vk_device::rateDeviceSuitability(VkPhysicalDevice device) {

        PFF_PROFILE_FUNCTION();

        VkPhysicalDeviceProperties  deviceProps;
        vkGetPhysicalDeviceProperties(device, &deviceProps);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // Application can't function without geometry shaders
        CORE_VALIDATE(deviceFeatures.geometryShader, return 0, "", "device deosnt support [GeometryShader]")

            int locScore = 0;
        if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            locScore += 1000;

        find_queue_families(device);

        // Maximum possible size of textures affects graphics quality
        locScore += deviceProps.limits.maxImageDimension2D;

        return locScore;
    }


    bool vk_device::check_validation_layer_support() {

        PFF_PROFILE_FUNCTION();

        u32 layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : m_validation_layers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> vk_device::get_required_extensions() {

        PFF_PROFILE_FUNCTION();

        u32 glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef PFF_DEBUG
        CORE_LOG(Info, "Number of extenstions required for GLFW: " << extensions.size());
        for (const char* name : extensions) {
            CORE_LOG(Trace, CONSOLE_LIST_BEGIN << name << "\"");
        }

        if (c_enable_validation_layers) {

            CORE_LOG(Info, "Number of extenstions required for validation_layer: " << 1);
            CORE_LOG(Trace, CONSOLE_LIST_BEGIN << VK_EXT_DEBUG_UTILS_EXTENSION_NAME << "\"");
        }
#endif

        if (c_enable_validation_layers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }


    void vk_device::has_required_Instance_extensions(const std::vector<const char*>& extentions) {

        PFF_PROFILE_FUNCTION();

        u32 extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

        LOG(Info, "available extensions:")
        std::unordered_set<std::string> available;
        for (const auto& extension : extensions) {

            available.insert(extension.extensionName);
            LOG(Trace, CONSOLE_LIST_BEGIN << extension.extensionName);
        }

        u32 layer_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &layer_count, nullptr);
        std::vector<VkExtensionProperties> layers(layer_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &layer_count, layers.data());

        LOG(Info, "available layers:")
        for (const auto& extension : layers) {

            available.insert(extension.extensionName);
            LOG(Trace, CONSOLE_LIST_BEGIN << extension.extensionName);
        }

        LOG(Info, "required extensions/layers:")
        for (const auto& required : extentions) {

            LOG(Trace, CONSOLE_LIST_BEGIN << required);
            CORE_ASSERT(available.find(required) != available.end(), "", "Missing required glfw extension");
        }
    }

    bool vk_device::check_device_extension_support(VkPhysicalDevice get_device) {

        PFF_PROFILE_FUNCTION();

        u32 extensionCount;
        vkEnumerateDeviceExtensionProperties(get_device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
            get_device,
            nullptr,
            &extensionCount,
            availableExtensions.data());

        std::set<std::string> requiredExtensions(m_device_extensions.begin(), m_device_extensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    vk_util::QueueFamilyIndices vk_device::find_queue_families(VkPhysicalDevice device) {

        PFF_PROFILE_FUNCTION();

        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {

                m_queue_family_indices.graphicsFamily = i;
                m_queue_family_indices.graphics_family_has_value = true;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport) {

                m_queue_family_indices.presentFamily = i;
                m_queue_family_indices.present_family_has_value = true;
            }

            if (m_queue_family_indices.isComplete())
                break;

            i++;
        }

        return m_queue_family_indices;
    }

    SwapChainSupportDetails vk_device::query_swapchain_support(VkPhysicalDevice get_device) {

        PFF_PROFILE_FUNCTION();

        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(get_device, m_surface, &details.capabilities);

        u32 format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(get_device, m_surface, &format_count, nullptr);

        if (format_count != 0) {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(get_device, m_surface, &format_count, details.formats.data());
        }

        u32 present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(get_device, m_surface, &present_mode_count, nullptr);

        if (present_mode_count != 0) {
            details.presentModes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(get_device, m_surface, &present_mode_count, details.presentModes.data());
        }
        return details;
    }

    VkFormat vk_device::find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

        PFF_PROFILE_FUNCTION();

        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (
                tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    u32 vk_device::find_memory_type(u32 typeFilter, VkMemoryPropertyFlags properties) {

        PFF_PROFILE_FUNCTION();

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memProperties);
        for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void vk_device::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& m_buffer, VkDeviceMemory& bufferMemory) {

        PFF_PROFILE_FUNCTION();

        VkBufferCreateInfo buffer_CI{};
        buffer_CI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_CI.size = size;
        buffer_CI.usage = usage;
        buffer_CI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        CORE_ASSERT(vkCreateBuffer(m_device, &buffer_CI, nullptr, &m_buffer) == VK_SUCCESS, "", "failed to create vertex buffer!");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

        VkMemoryAllocateInfo alloc_CI{};
        alloc_CI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_CI.allocationSize = memRequirements.size;
        alloc_CI.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);

        CORE_ASSERT(vkAllocateMemory(m_device, &alloc_CI, nullptr, &bufferMemory) == VK_SUCCESS, "", "failed to allocate vertex buffer memory!");

        vkBindBufferMemory(m_device, m_buffer, bufferMemory, 0);
    }

    VkCommandBuffer vk_device::begin_single_time_commands() {

        PFF_PROFILE_FUNCTION();

        VkCommandBufferAllocateInfo alloc_CI{};
        alloc_CI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_CI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_CI.commandPool = m_commandPool;
        alloc_CI.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_device, &alloc_CI, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void vk_device::end_single_time_commands(VkCommandBuffer commandBuffer) {

        PFF_PROFILE_FUNCTION();

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_graphics_queue);

        vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
    }

    void vk_device::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

        PFF_PROFILE_FUNCTION();

        VkCommandBuffer commandBuffer = begin_single_time_commands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        end_single_time_commands(commandBuffer);
    }

    void vk_device::copy_buffer_to_image( VkBuffer m_buffer, VkImage image, u32 width, u32 height, u32 layerCount) {

        PFF_PROFILE_FUNCTION();

        VkCommandBuffer commandBuffer = begin_single_time_commands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(commandBuffer, m_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        end_single_time_commands(commandBuffer);
    }

    void vk_device::create_image_with_info( const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {

        PFF_PROFILE_FUNCTION();

        CORE_ASSERT(vkCreateImage(m_device, &imageInfo, nullptr, &image) == VK_SUCCESS, "", "failed to create image!")

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);

        CORE_ASSERT(vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) == VK_SUCCESS, "", "failed to allocate image memory!");

        CORE_ASSERT(vkBindImageMemory(m_device, image, imageMemory, 0) == VK_SUCCESS, "", "failed to bind image memory!");
    }

    const char* vk_debug_message_type_to_string(VkDebugUtilsMessageTypeFlagsEXT flag) {

        switch (flag) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            return "[general info] ";

        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            return "[validation info] ";

        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            return "[performance info] ";

        case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
            return "[device adress binding info] ";

        case VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT:
            return "[MAX ENUM VALUE] ";

        default:
            return "[Unknown type] ";
        }

    }

    const char* physical_device_type_to_string(VkPhysicalDeviceType type) {

        switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "[other]";

        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "[integrated GPU]";

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "[discrete GPU]";

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "[virtual GPU]";

        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "[CPU]";

        default:
            return "[no type found]";
        }

    }

}
