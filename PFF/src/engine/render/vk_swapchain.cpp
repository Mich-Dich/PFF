
#include "util/pffpch.h"

#include "vk_swapchain.h"
#include "engine/render/vk_device.h"
#include "engine/render/queue_families.h"

namespace PFF {

    vk_swapchain::vk_swapchain(ref<vk_device>& device, VkExtent2D window_extent)
        : m_device{ device }, m_window_extent{ window_extent } {

        PFF_PROFILE_FUNCTION();

        init();
    }

    vk_swapchain::vk_swapchain(ref<vk_device>& device, VkExtent2D window_extent, ref<vk_swapchain> previous)
        : m_device{ device }, m_window_extent{ window_extent }, m_old_swapchain{ previous } {

        PFF_PROFILE_FUNCTION();

        init();
        m_old_swapchain.reset();      // clean up  old swapchain, not needed anymore
        LOG(Trace, "init");
    }

    vk_swapchain::~vk_swapchain() {

        PFF_PROFILE_FUNCTION();

        for (auto imageView : m_swap_chain_image_views) {
            vkDestroyImageView(m_device->get_device(), imageView, nullptr);
        }
        m_swap_chain_image_views.clear();

        if (m_swap_chain != nullptr) {
            vkDestroySwapchainKHR(m_device->get_device(), m_swap_chain, nullptr);
            m_swap_chain = nullptr;
        }

        for (int i = 0; i < m_depth_images.size(); i++) {
            vkDestroyImageView(m_device->get_device(), m_depth_image_views[i], nullptr);
            vkDestroyImage(m_device->get_device(), m_depth_images[i], nullptr);
            vkFreeMemory(m_device->get_device(), m_depth_image_memorys[i], nullptr);
        }

        for (auto framebuffer : m_swap_chain_framebuffers) {
            vkDestroyFramebuffer(m_device->get_device(), framebuffer, nullptr);
        }

        vkDestroyRenderPass(m_device->get_device(), m_render_pass, nullptr);

        // cleanup synchronization objects
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_device->get_device(), m_render_finished_semaphores[i], nullptr);
            vkDestroySemaphore(m_device->get_device(), m_image_available_semaphores[i], nullptr);
            vkDestroyFence(m_device->get_device(), m_in_flight_fences[i], nullptr);
        }

        m_old_swapchain.reset();
        m_device.reset();
        LOG(Trace, "shutdown");
    }

    VkResult vk_swapchain::acquireNextImage(u32* imageIndex) {

        PFF_PROFILE_FUNCTION();

        vkWaitForFences(m_device->get_device(), 1, &m_in_flight_fences[m_current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        VkResult result = vkAcquireNextImageKHR(
            m_device->get_device(), 
            m_swap_chain, 
            std::numeric_limits<uint64_t>::max(), 
            m_image_available_semaphores[m_current_frame],  // must be a not signaled semaphore
            VK_NULL_HANDLE,
            imageIndex);

        *imageIndex = *imageIndex % MAX_FRAMES_IN_FLIGHT;
        return result;
    }

    VkResult vk_swapchain::submitCommandBuffers(const VkCommandBuffer* buffers, u32* imageIndex) {

        PFF_PROFILE_FUNCTION();

        if (m_images_in_flight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(m_device->get_device(), 1, &m_images_in_flight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        m_images_in_flight[*imageIndex] = m_in_flight_fences[m_current_frame];

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_image_available_semaphores[m_current_frame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        VkSemaphore signalSemaphores[] = { m_render_finished_semaphores[m_current_frame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(m_device->get_device(), 1, &m_in_flight_fences[m_current_frame]);
        CORE_ASSERT(vkQueueSubmit(m_device->get_graphics_queue(), 1, &submitInfo, m_in_flight_fences[m_current_frame]) == VK_SUCCESS, "", "failed to submit draw command buffer!");

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { m_swap_chain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = imageIndex;

        auto result = vkQueuePresentKHR(m_device->get_present_queue(), &presentInfo);

        m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }


    void vk_swapchain::init() {

        PFF_PROFILE_FUNCTION();

        createSwapChain();
        createImageViews();
        create_main_render_pass(m_device->get_device(), nullptr);
        create_ui_render_pass(m_device->get_device(), nullptr);
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }

    void vk_swapchain::createSwapChain() {

        PFF_PROFILE_FUNCTION();

        m_swapchain_support = m_device->get_swap_chain_support();
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(m_swapchain_support.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(m_swapchain_support.presentModes);
        VkExtent2D extent = chooseSwapExtent(m_swapchain_support.capabilities);

        u32 imageCount = m_swapchain_support.capabilities.minImageCount + 1;
        if (m_swapchain_support.capabilities.maxImageCount > 0 && imageCount > m_swapchain_support.capabilities.maxImageCount)
            imageCount = m_swapchain_support.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_device->get_surface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        vk_util::QueueFamilyIndices indices = m_device->find_physical_queue_families();

        u32 queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };
        if (indices.graphicsFamily != indices.presentFamily) {                          // graphics and present are diffrent queues

            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {                                                                        // graphics and present are the same queue
            
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = m_swapchain_support.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = (m_old_swapchain == nullptr) ? VK_NULL_HANDLE : m_old_swapchain->m_swap_chain;

        CORE_ASSERT(vkCreateSwapchainKHR(m_device->get_device(), &createInfo, nullptr, &m_swap_chain) == VK_SUCCESS, "", "failed to create swap chain!");

        // we only specified a minimum number of images in the swap chain, so the implementation is
        // allowed to create a swap chain with more. That's why we'll first query the final number of
        // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
        // retrieve the handles.
        vkGetSwapchainImagesKHR(m_device->get_device(), m_swap_chain, &imageCount, nullptr);
        m_swap_chain_images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_device->get_device(), m_swap_chain, &imageCount, m_swap_chain_images.data());

        m_swap_chainImage_format = surfaceFormat.format;
        m_swap_chain_extent = extent;
    }

    void vk_swapchain::createImageViews() {

        PFF_PROFILE_FUNCTION();

        m_swap_chain_image_views.resize(m_swap_chain_images.size());
        for (size_t i = 0; i < m_swap_chain_images.size(); i++) {
              
            VkImageViewCreateInfo image_view_CI{};
            image_view_CI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_CI.image = m_swap_chain_images[i];
            image_view_CI.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_CI.format = m_swap_chainImage_format;
            image_view_CI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_CI.subresourceRange.baseMipLevel = 0;
            image_view_CI.subresourceRange.levelCount = 1;
            image_view_CI.subresourceRange.baseArrayLayer = 0;
            image_view_CI.subresourceRange.layerCount = 1;

            CORE_ASSERT(vkCreateImageView(m_device->get_device(), &image_view_CI, nullptr, &m_swap_chain_image_views[i]) == VK_SUCCESS, "", "failed to create texture image view!");
        }
    }

    void vk_swapchain::create_main_render_pass(VkDevice device, const VkAllocationCallbacks* allocator) {

        PFF_PROFILE_FUNCTION();

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = get_swapchain_image_format();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<u32>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        CORE_ASSERT(vkCreateRenderPass(m_device->get_device(), &renderPassInfo, nullptr, &m_render_pass) == VK_SUCCESS, "", "failed to create render pass!");
    }

    void vk_swapchain::create_ui_render_pass(VkDevice device, const VkAllocationCallbacks* allocator) {

        PFF_PROFILE_FUNCTION();

        VkAttachmentDescription attachment = {};
        attachment.format = get_swapchain_image_format();
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        VkAttachmentReference color_attachment = {};
        color_attachment.attachment = 0;
        color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &attachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        CORE_ASSERT(vkCreateRenderPass(m_device->get_device(), &renderPassInfo, nullptr, &m_ui_render_pass) == VK_SUCCESS, "", "failed to create render pass!");
    }

    void vk_swapchain::createFramebuffers() {

        PFF_PROFILE_FUNCTION();

        m_swap_chain_framebuffers.resize(get_image_count());
        for (size_t i = 0; i < get_image_count(); i++) {

            std::array<VkImageView, 2> attachments = { m_swap_chain_image_views[i], m_depth_image_views[i] };

            VkExtent2D m_swap_chain_extent = get_swapchain_extent();
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_render_pass;
            framebufferInfo.attachmentCount = static_cast<u32>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_swap_chain_extent.width;
            framebufferInfo.height = m_swap_chain_extent.height;
            framebufferInfo.layers = 1;

            CORE_ASSERT(vkCreateFramebuffer(m_device->get_device(), &framebufferInfo, nullptr, &m_swap_chain_framebuffers[i]) == VK_SUCCESS, "", "failed to create framebuffer!");
        }
    }

    void vk_swapchain::createDepthResources() {

        PFF_PROFILE_FUNCTION();

        VkFormat depthFormat = findDepthFormat();
        VkExtent2D m_swap_chain_extent = get_swapchain_extent();

        m_depth_images.resize(get_image_count());
        m_depth_image_memorys.resize(get_image_count());
        m_depth_image_views.resize(get_image_count());

        for (int i = 0; i < m_depth_images.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = m_swap_chain_extent.width;
            imageInfo.extent.height = m_swap_chain_extent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            m_device->create_image_with_info(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_images[i], m_depth_image_memorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_depth_images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            CORE_ASSERT(vkCreateImageView(m_device->get_device(), &viewInfo, nullptr, &m_depth_image_views[i]) == VK_SUCCESS, "", "failed to create texture image view!");
        }
    }

    void vk_swapchain::createSyncObjects() {

        PFF_PROFILE_FUNCTION();

        m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
        m_images_in_flight.resize(get_image_count(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            CORE_ASSERT(vkCreateSemaphore(m_device->get_device(), &semaphoreInfo, nullptr, &m_image_available_semaphores[i]) == VK_SUCCESS &&
                vkCreateSemaphore(m_device->get_device(), &semaphoreInfo, nullptr, &m_render_finished_semaphores[i]) == VK_SUCCESS &&
                vkCreateFence(m_device->get_device(), &fenceInfo, nullptr, &m_in_flight_fences[i]) == VK_SUCCESS, "", "failed to create synchronization objects for a frame!");
        }
    }

    VkSurfaceFormatKHR vk_swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

        PFF_PROFILE_FUNCTION();

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR vk_swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

        PFF_PROFILE_FUNCTION();

        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {

                LOG(Trace, "Present mode: Mailbox");
                return availablePresentMode;
            }
            
        }
        
        /*
        auto mode = availablePresentModes[0];

        switch (mode) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            CORE_LOG(Trace, "Present Modes: Immediat Mode")
                break;

        case VK_PRESENT_MODE_MAILBOX_KHR:
            CORE_LOG(Trace, "Present Modes: Mailbox")
                break;
        case VK_PRESENT_MODE_FIFO_KHR:
            CORE_LOG(Trace, "Present Modes: FiFo")
                break;

        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            CORE_LOG(Trace, "Present Modes: FiFo relaxed")
                break;

        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
            CORE_LOG(Trace, "Present Modes: Shared Demand Refresh")
                break;

        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            CORE_LOG(Trace, "Present Modes: Shared Continues Refresh")
                break;

        case VK_PRESENT_MODE_MAX_ENUM_KHR:
            CORE_LOG(Trace, "Present Modes: MAX ENUM")
                break;

        default:
            break;
        }
        */

        return VK_PRESENT_MODE_FIFO_KHR;        // Must alwais be suported
    }

    VkExtent2D vk_swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

        PFF_PROFILE_FUNCTION();

        if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
            return capabilities.currentExtent;

        else {

            VkExtent2D actualExtent = m_window_extent;
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            // actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            // actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VkFormat vk_swapchain::findDepthFormat() {

        PFF_PROFILE_FUNCTION();

        return m_device->find_supported_format(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    const char* present_mode_to_str(const VkPresentModeKHR mode) {

        switch (mode) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return "Immediat";

        case VK_PRESENT_MODE_MAILBOX_KHR:
            return "Mailbox";

        case VK_PRESENT_MODE_FIFO_KHR:
            return "FiFo";

        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            return "Fifo Relaxed";

        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
            return "Shared Demand Refresh";

        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            return "Shared Continues Refresh";

        default:
            return "Unknown";
        }
    }

}
