#pragma once

#include "util/util.h"

// vulkan headers
#include <vulkan/vulkan.h>

namespace PFF {

    class vk_device;

    class vk_swapchain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        vk_swapchain(std::shared_ptr<vk_device>& device, VkExtent2D m_window_extent);
        ~vk_swapchain();

        vk_swapchain(const vk_swapchain&) = delete;
        void operator=(const vk_swapchain&) = delete;
        VkFormat findDepthFormat();

        VkFramebuffer getFrameBuffer(int index) { return m_swap_chain_framebuffers[index]; }
        VkImageView getImageView(int index) { return m_swap_chain_image_views[index]; }
        size_t imageCount() { return m_swap_chain_images.size(); }
        VkFormat getSwapChainImageFormat() { return m_swap_chainImage_format; }
        VkExtent2D getSwapChainExtent() { return m_swap_chain_extent; }

        inline VkRenderPass get_render_pass() { return m_render_pass; }
        inline u32 get_width() { return m_swap_chain_extent.width; }
        inline u32 get_height() { return m_swap_chain_extent.height; }
        inline f32 get_extentAspectRatio() { return static_cast<float>(m_swap_chain_extent.width) / static_cast<float>(m_swap_chain_extent.height); }

        VkResult acquireNextImage(u32* imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, u32* imageIndex);

    private:
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkFormat m_swap_chainImage_format;
        VkExtent2D m_swap_chain_extent;

        std::vector<VkFramebuffer> m_swap_chain_framebuffers;
        VkRenderPass m_render_pass;

        std::vector<VkImage> m_depth_images;
        std::vector<VkDeviceMemory> m_depth_image_memorys;
        std::vector<VkImageView> m_depth_image_views;
        std::vector<VkImage> m_swap_chain_images;
        std::vector<VkImageView> m_swap_chain_image_views;

        std::shared_ptr<vk_device> m_device;
        VkExtent2D m_window_extent;

        VkSwapchainKHR m_swap_chain;

        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence> m_in_flight_fences;
        std::vector<VkFence> m_images_in_flight;
        size_t m_current_frame = 0;
    };

}
