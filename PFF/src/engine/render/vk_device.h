#pragma once

#include <vulkan/vulkan.h>
#include "engine/render/queue_families.h"
#include "engine/render/vk_swapchain.h"

namespace PFF {

    static const char* vk_debug_message_type_to_string(VkDebugUtilsMessageTypeFlagsEXT flag);
    static const char* physical_device_type_to_string(VkPhysicalDeviceType type);

    class pff_window;
    // struct SwapChainSupportDetails;


    class vk_device {

    public:

#if defined(PFF_DEBUG) || defined(PFF_RELEASE_WITH_DEBUG_INFO)
        const bool c_enable_validation_layers = true;
#else
        const bool c_enable_validation_layers = false;
#endif

        vk_device(std::shared_ptr<pff_window>& m_window);
        ~vk_device();

        // Not copyable or movable
        DELETE_COPY_MOVE(vk_device);

        FORCEINLINE VkCommandPool get_command_pool() const { return m_commandPool; }
        FORCEINLINE VkDevice get_device() const { return m_device; }
        FORCEINLINE VkSurfaceKHR get_surface() const { return m_surface; }
        FORCEINLINE VkQueue get_graphics_queue() const { return m_graphics_queue; }
        FORCEINLINE VkQueue get_present_queue() const { return m_present_queue; }
        FORCEINLINE vk_util::QueueFamilyIndices get_queue_families() const { return m_queue_family_indices; }

        // TEMPORARY
        FORCEINLINE VkInstance get_instance() const { return m_VkInstance; }
        FORCEINLINE VkPhysicalDevice get_physical_device() const { return m_physical_device; }


        SwapChainSupportDetails get_swap_chain_support() { return query_swapchain_support(m_physical_device); }
        vk_util::QueueFamilyIndices find_physical_queue_families() { return find_queue_families(m_physical_device); }
        uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkFormat find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        VkCommandBuffer begin_single_time_commands();
        void end_single_time_commands(VkCommandBuffer commandBuffer);
        void create_buffer( VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& m_buffer, VkDeviceMemory& bufferMemory);
        void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copy_buffer_to_image( VkBuffer m_buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
        void create_image_with_info(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

        VkPhysicalDeviceProperties properties;

    private:
        void create_instance();
        void create_surface();
        void pick_physical_device();
        void create_logical_device();
        void create_command_pool();

        // - - - - - - - - - - - - - Debug messenger - - - - - - - - - - - - - 
        void setup_debug_messenger();
        VkResult create_debug_utils_messengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void destroy_debug_utils_messengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        void populate_debug_messenger_CI(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* Callback, void* UserData);

        // helper functions
        bool is_device_suitable(VkPhysicalDevice get_device);
        int rateDeviceSuitability(VkPhysicalDevice device);

        // @brief Retrieves the required Vulkan_Instance extension_names for GLFW and Validation_Layer (if enabled)
        // @return A vector containing the names fo the required extensions
        std::vector<const char*> get_required_extensions();
        bool check_validation_layer_support();
        vk_util::QueueFamilyIndices find_queue_families(VkPhysicalDevice get_device);
        void has_required_Instance_extensions(const std::vector<const char*>& extentions);
        bool check_device_extension_support(VkPhysicalDevice get_device);
        SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice get_device);

        bool check_for_WSI_support();

        vk_util::QueueFamilyIndices m_queue_family_indices;
        VkInstance m_VkInstance;
        VkDebugUtilsMessengerEXT m_debug_messanger;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        std::shared_ptr<pff_window> m_window;
        VkCommandPool m_commandPool;

        VkDevice m_device;
        VkSurfaceKHR m_surface;
        VkQueue m_graphics_queue;
        VkQueue m_present_queue;

        const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> m_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };

}
