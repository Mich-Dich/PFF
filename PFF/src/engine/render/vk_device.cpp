
#include "engine/platform/pff_window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk_device.h"

namespace PFF {


    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // - - - - - Setup
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    // class member functions
    vk_device::vk_device(std::shared_ptr<pff_window>& m_window) : m_window{ m_window } {

        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
    }

    vk_device::~vk_device() {

        LOG(Info, "Destroying vk_device");
        vkDestroyCommandPool(m_device, commandPool, nullptr);
        vkDestroyDevice(m_device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_VkInstance, m_debug_messanger, nullptr);
        }

        vkDestroySurfaceKHR(m_VkInstance, m_surface, nullptr);
        vkDestroyInstance(m_VkInstance, nullptr);
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // - - - - - Debug messenger functions
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //
    void vk_device::setupDebugMessenger() {

        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        CORE_VALIDATE(CreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, nullptr, &m_debug_messanger) == VK_SUCCESS, "Successfully set up debug messanger", "FAILED to set up debug messenger", return);
    }

    //
    VkResult vk_device::CreateDebugUtilsMessengerEXT( VkInstance m_VkInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(m_VkInstance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    //
    void vk_device::DestroyDebugUtilsMessengerEXT(VkInstance m_VkInstance, VkDebugUtilsMessengerEXT m_debug_messanger, const VkAllocationCallbacks* pAllocator) {

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(m_VkInstance, m_debug_messanger, pAllocator);
        }
    }

    //
    void vk_device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {

        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;
    }

    //
    VKAPI_ATTR VkBool32 VKAPI_CALL vk_device::DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
        VkDebugUtilsMessageTypeFlagsEXT messageType, 
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbaclData, 
        void* pUserData) {

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
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {

            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)		CORE_LOG(Trace, "Validation Layer: " << pCallbaclData->pMessage);
            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)			CORE_LOG(Debug, "Validation Layer: " << pCallbaclData->pMessage);
            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)		CORE_LOG(Warn, "Validation Layer: " << pCallbaclData->pMessage);
            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)		CORE_LOG(Error, "Validation Layer: " << pCallbaclData->pMessage);
        }

        return VK_FALSE;
    }



    VkResult CreateDebugUtilsMessengerEXT( VkInstance m_VkInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(m_VkInstance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT( VkInstance m_VkInstance, VkDebugUtilsMessengerEXT m_debug_messanger, const VkAllocationCallbacks* pAllocator) {

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(m_VkInstance, m_debug_messanger, pAllocator);
        }
    }


    void vk_device::createInstance() {

        CORE_ASSERT(!(enableValidationLayers && !checkValidationLayerSupport()),"", "validation layers requested, but not available!");

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "LittleVulkanEngine App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {

            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

        } else {

            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        CORE_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_VkInstance) == VK_SUCCESS, "", "failed to create m_VkInstance!");
       
        hasGflwRequiredInstanceExtensions();
    }

    void vk_device::pickPhysicalDevice() {

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
        CORE_ASSERT(deviceCount != 0, "", "failed to find GPUs with Vulkan support!");

        LOG(Info, "Device count: " << deviceCount);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

        for (const auto& get_device : devices) {
            if (isDeviceSuitable(get_device)) {
                m_physical_device = get_device;
                break;
            }
        }

        CORE_ASSERT(m_physical_device != VK_NULL_HANDLE, "", "failed to find a suitable GPU!");

        vkGetPhysicalDeviceProperties(m_physical_device, &properties);
        LOG(Info, "physical get_device: " << properties.deviceName);
    }

    void vk_device::createLogicalDevice() {

        QueueFamilyIndices indices = findQueueFamilies(m_physical_device);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        // might not really be necessary anymore because device specific validation layers
        // have been deprecated
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        CORE_ASSERT(vkCreateDevice(m_physical_device, &createInfo, nullptr, &m_device) == VK_SUCCESS, "", "failed to create logical get_device!");

        vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &graphicsQueue_);
        vkGetDeviceQueue(m_device, indices.presentFamily, 0, &presentQueue_);
    }

    void vk_device::createCommandPool() {

        QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        CORE_ASSERT(vkCreateCommandPool(m_device, &poolInfo, nullptr, &commandPool) == VK_SUCCESS, "", "failed to create command pool!");
    }

    void vk_device::createSurface() { m_window->createWindowSurface(m_VkInstance, &m_surface); }

    bool vk_device::isDeviceSuitable(VkPhysicalDevice device) {

        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate &&
            supportedFeatures.samplerAnisotropy;
    }

    bool vk_device::checkValidationLayerSupport() {

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
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

    std::vector<const char*> vk_device::getRequiredExtensions() {

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void vk_device::hasGflwRequiredInstanceExtensions() {

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        LOG(Info, "available extensions:")
        std::unordered_set<std::string> available;
        for (const auto& extension : extensions) {

            LOG(Trace, std::setw(40) << extension.extensionName);
            available.insert(extension.extensionName);
        }

        LOG(Info, "required extensions:")
        auto requiredExtensions = getRequiredExtensions();
        for (const auto& required : requiredExtensions) {

            LOG(Trace, std::setw(40) << required);
            CORE_ASSERT(available.find(required) != available.end(), "", "Missing required glfw extension");
        }
    }

    bool vk_device::checkDeviceExtensionSupport(VkPhysicalDevice get_device) {

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(get_device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
            get_device,
            nullptr,
            &extensionCount,
            availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices vk_device::findQueueFamilies(VkPhysicalDevice get_device) {

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(get_device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(get_device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
                indices.graphicsFamilyHasValue = true;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(get_device, i, m_surface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport) {
                indices.presentFamily = i;
                indices.presentFamilyHasValue = true;
            }
            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    SwapChainSupportDetails vk_device::querySwapChainSupport(VkPhysicalDevice get_device) {

        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(get_device, m_surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(get_device, m_surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(get_device, m_surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(get_device, m_surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                get_device,
                m_surface,
                &presentModeCount,
                details.presentModes.data());
        }
        return details;
    }

    VkFormat vk_device::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

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

    uint32_t vk_device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void vk_device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

        VkBufferCreateInfo buffer_CI{};
        buffer_CI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_CI.size = size;
        buffer_CI.usage = usage;
        buffer_CI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        CORE_ASSERT(vkCreateBuffer(m_device, &buffer_CI, nullptr, &buffer) == VK_SUCCESS, "", "failed to create vertex buffer!");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

        VkMemoryAllocateInfo alloc_CI{};
        alloc_CI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_CI.allocationSize = memRequirements.size;
        alloc_CI.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        CORE_ASSERT(vkAllocateMemory(m_device, &alloc_CI, nullptr, &bufferMemory) == VK_SUCCESS, "", "failed to allocate vertex buffer memory!");

        vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
    }

    VkCommandBuffer vk_device::beginSingleTimeCommands() {

        VkCommandBufferAllocateInfo alloc_CI{};
        alloc_CI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_CI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_CI.commandPool = commandPool;
        alloc_CI.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_device, &alloc_CI, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void vk_device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue_);

        vkFreeCommandBuffers(m_device, commandPool, 1, &commandBuffer);
    }

    void vk_device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void vk_device::copyBufferToImage( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

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

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);
        endSingleTimeCommands(commandBuffer);
    }

    void vk_device::createImageWithInfo( const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        
        CORE_ASSERT(vkCreateImage(m_device, &imageInfo, nullptr, &image) == VK_SUCCESS, "", "failed to create image!")

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        CORE_ASSERT(vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) == VK_SUCCESS, "", "failed to allocate image memory!");

        CORE_ASSERT(vkBindImageMemory(m_device, image, imageMemory, 0) == VK_SUCCESS, "", "failed to bind image memory!");
    }

}
