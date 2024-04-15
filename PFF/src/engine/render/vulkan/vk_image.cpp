
#include "util/pffpch.h"

#include "vk_types.h"
#include "vk_initializers.h"

#include "vk_image.h"

namespace PFF::render::vulkan::util {

    void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout) {

        VkImageAspectFlags aspect_mask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
                
        VkImageMemoryBarrier2 image_barrier{};
        image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        image_barrier.pNext = nullptr;
        image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            // TIPP: ALL_COMMANDS is inefficient, as it will stall the GPU pipeline a bit. => OK for current needs, as we are only going to do a few transitions per frame.
            //       For many transitions per frame as part of a post-process chain, should avoid doing this, and instead use StageMasks more accurate to what needs to be done
        image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            // TIPP: ALL_COMMANDS is inefficient, as it will stall the GPU pipeline a bit. => OK for current needs, as we are only going to do a few transitions per frame.
            //       For many transitions per frame as part of a post-process chain, should avoid doing this, and instead use StageMasks more accurate to what needs to be done
        image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
        image_barrier.oldLayout = current_layout;
        image_barrier.newLayout = new_layout;
        image_barrier.subresourceRange = PFF::render::vulkan::init::image_subresource_range(aspect_mask);
        image_barrier.image = image;

        // TIPP: can send multiple imageMemoryBarriers, to improve performance
        VkDependencyInfo dependency_I{};
        dependency_I.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency_I.pNext = nullptr;
        dependency_I.imageMemoryBarrierCount = 1;
        dependency_I.pImageMemoryBarriers = &image_barrier;

        vkCmdPipelineBarrier2(cmd, &dependency_I);
    }

    void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize) {
    
        VkImageBlit2 blit_region{};
        blit_region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
        blit_region.pNext = nullptr;

        blit_region.srcOffsets[1].x = srcSize.width;
        blit_region.srcOffsets[1].y = srcSize.height;
        blit_region.srcOffsets[1].z = 1;
        blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit_region.srcSubresource.baseArrayLayer = 0;
        blit_region.srcSubresource.layerCount = 1;
        blit_region.srcSubresource.mipLevel = 0;

        blit_region.dstOffsets[1].x = dstSize.width;
        blit_region.dstOffsets[1].y = dstSize.height;
        blit_region.dstOffsets[1].z = 1;
        blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit_region.dstSubresource.baseArrayLayer = 0;
        blit_region.dstSubresource.layerCount = 1;
        blit_region.dstSubresource.mipLevel = 0;

        VkBlitImageInfo2 blit_I{};
        blit_I.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
        blit_I.pNext = nullptr;
        blit_I.dstImage = destination;
        blit_I.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blit_I.srcImage = source;
        blit_I.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blit_I.filter = VK_FILTER_LINEAR;
        blit_I.regionCount = 1;
        blit_I.pRegions = &blit_region;

        vkCmdBlitImage2(cmd, &blit_I);
    }

}
