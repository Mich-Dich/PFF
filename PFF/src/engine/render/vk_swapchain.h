#pragma once

// vulkan headers
#include <vulkan/vulkan.h>

namespace PFF {

    class vk_device;

    class vk_swapchain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 3;

        vk_swapchain(std::shared_ptr<vk_device>& device, VkExtent2D window_extent);
        vk_swapchain(std::shared_ptr<vk_device>& device, VkExtent2D window_extent, std::shared_ptr<vk_swapchain> previous);
        ~vk_swapchain();

        DELETE_COPY(vk_swapchain);

        VkFormat findDepthFormat();
        VkResult acquireNextImage(u32* imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, u32* imageIndex);

        FORCEINLINE VkFramebuffer getFrameBuffer(const int index)  const { return m_swap_chain_framebuffers[index]; }
        FORCEINLINE VkImageView get_image_view(int index)  const { return m_swap_chain_image_views[index]; }
        FORCEINLINE VkFormat get_swapchain_image_format()  const { return m_swap_chainImage_format; }
        FORCEINLINE VkExtent2D get_swapchain_extent() const { return m_swap_chain_extent; }
        FORCEINLINE u32 get_image_count() const { return static_cast<u32>(m_swap_chain_images.size()); }
        FORCEINLINE u32 get_max_frames_in_flight() const { return static_cast<u32>(MAX_FRAMES_IN_FLIGHT); }
        FORCEINLINE VkRenderPass get_render_pass()  const { return m_render_pass; }
        FORCEINLINE u32 get_width() const { return m_swap_chain_extent.width; }
        FORCEINLINE u32 get_height() const { return m_swap_chain_extent.height; }
        FORCEINLINE f32 get_extentAspectRatio() const { return static_cast<float>(m_swap_chain_extent.width) / static_cast<float>(m_swap_chain_extent.height); }

    private:

        void init();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

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
        std::shared_ptr<vk_swapchain> m_old_swapchain;

        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence> m_in_flight_fences;
        std::vector<VkFence> m_images_in_flight;
        size_t m_current_frame = 0;
    };

}
