
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

}
