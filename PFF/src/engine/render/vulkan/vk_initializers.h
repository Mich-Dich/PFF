#pragma once

namespace PFF::render::vulkan::init {

    //@brief Initializes a command pool with specified properties.
    //@param [queueFamilyIndex] The index of the queue family to create the command pool for.
    //@param [flags] The command pool creation flags. Default value: 0.
    //@return VkCommandPoolCreateInfo structure initialized with the provided parameters.
    VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

    //@brief Initializes a command buffer allocation info with specified properties.
    //@param [pool] The command pool from which the command buffers will be allocated.
    //@param [count] The number of command buffers to allocate. Default value: 1.
    //@return VkCommandBufferAllocateInfo structure initialized with the provided parameters.
    VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1);
    
    //@brief Initializes a command buffer begin info with specified properties.
    //@param [flags] The command buffer usage flags. Default value: 0.
    //@return VkCommandBufferBeginInfo structure initialized with the provided parameters.
    VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);

    //@brief Creates a fence with specified properties.
    //@param [flags] The fence creation flags. Default value: 0.
    //@return VkFenceCreateInfo structure initialized with the provided parameters.
    VkCommandBufferSubmitInfo command_buffer_submit_info(VkCommandBuffer cmd);

    //@brief Creates a semaphore with specified properties.
    //@param [flags] The semaphore creation flags. Default value: 0.
    //@return VkSemaphoreCreateInfo structure initialized with the provided parameters.
    VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

    //@brief Initializes a semaphore submission info with specified properties.
    //@param [stageMask] The pipeline stage mask.
    //@param [semaphore] The semaphore to submit.
    //@return VkSemaphoreSubmitInfo structure initialized with the provided parameters.
    VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

    //@brief Initializes a submit info with specified properties.
    //@param [cmd] Pointer to a VkCommandBufferSubmitInfo structure.
    //@param [signalSemaphoreInfo] Pointer to a VkSemaphoreSubmitInfo structure for signal semaphore info.
    //@param [waitSemaphoreInfo] Pointer to a VkSemaphoreSubmitInfo structure for wait semaphore info.
    //@return VkSubmitInfo2 structure initialized with the provided parameters.
    VkSubmitInfo2 submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo);

    //@brief Initializes a present info with default values.
    //@return VkPresentInfoKHR structure initialized with default values.
    VkPresentInfoKHR present_info();

    //@brief Initializes a rendering attachment info for color with specified properties.
    //@param [view] The image view.
    //@param [clear] Pointer to the clear value. Default value: nullptr.
    //@param [layout] The image layout. Default value: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL.
    //@return VkRenderingAttachmentInfo structure initialized with the provided parameters.
    VkRenderingAttachmentInfo attachment_info(VkImageView view, VkClearValue* clear, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

    //@brief Initializes a rendering attachment info for depth with specified properties.
    //@param [view] The image view.
    //@param [layout] The image layout. Default value: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL.
    //@return VkRenderingAttachmentInfo structure initialized with the provided parameters.
    VkRenderingAttachmentInfo depth_attachment_info(VkImageView view, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

    //@brief Initializes a rendering info with specified properties.
     //@param [renderExtent] The extent of the render area.
     //@param [colorAttachment] Pointer to a VkRenderingAttachmentInfo structure for color attachment info.
     //@param [depthAttachment] Pointer to a VkRenderingAttachmentInfo structure for depth attachment info.
     //@return VkRenderingInfo structure initialized with the provided parameters.
    VkRenderingInfo rendering_info(VkExtent2D renderExtent, VkRenderingAttachmentInfo* colorAttachment, VkRenderingAttachmentInfo* depthAttachment);

    //@brief Initializes an image subresource range with specified properties.
    //@param [aspectMask] The aspect mask.
    //@return VkImageSubresourceRange structure initialized with the provided parameters.
    VkImageSubresourceRange image_subresource_range(VkImageAspectFlags aspectMask);

    //@brief Creates a VkSemaphoreSubmitInfo struct with specified parameters.
    //@brief This function initializes a VkSemaphoreSubmitInfo struct with the provided stage mask and semaphore.
    //@param stageMask The stage mask specifying the pipeline stages at which the semaphore waits.
    //@param semaphore The VkSemaphore object to be used in the semaphore submit info
    VkSemaphoreSubmitInfo semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
    
    //@brief Initializes a descriptor set layout binding with specified properties.
    //@param [type] The type of the descriptor.
    //@param [stageFlags] The shader stages that can access the descriptor.
    //@param [binding] The binding number.
    //@return VkDescriptorSetLayoutBinding structure initialized with the provided parameters.
    VkDescriptorSetLayoutBinding descriptorset_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding);
    
    //@brief Initializes a descriptor set layout create info with specified properties.
    //@param [bindings] Pointer to an array of VkDescriptorSetLayoutBinding structures.
    //@param [bindingCount] The number of descriptor bindings.
    //@return VkDescriptorSetLayoutCreateInfo structure initialized with the provided parameters.
    VkDescriptorSetLayoutCreateInfo descriptorset_layout_create_info(VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount);
    
    //@brief Initializes a write descriptor set for images with specified properties.
    //@param [type] The type of the descriptor.
    //@param [dstSet] The destination descriptor set.
    //@param [imageInfo] Pointer to an array of VkDescriptorImageInfo structures.
    //@param [binding] The binding number.
    //@return VkWriteDescriptorSet structure initialized with the provided parameters.
    VkWriteDescriptorSet write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding);
    
    //@brief Initializes a write descriptor set for buffers with specified properties.
    //@param [type] The type of the descriptor.
    //@param [dstSet] The destination descriptor set.
    //@param [bufferInfo] Pointer to an array of VkDescriptorBufferInfo structures.
    //@param [binding] The binding number.
    //@return VkWriteDescriptorSet structure initialized with the provided parameters.
    VkWriteDescriptorSet write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding);
    
    //@brief Initializes a descriptor buffer info with specified properties.
    //@param [buffer] The buffer resource.
    //@param [offset] The starting offset in bytes from the beginning of buffer.
    //@param [range] The size in bytes of the buffer resource.
    //@return VkDescriptorBufferInfo structure initialized with the provided parameters.
    VkDescriptorBufferInfo buffer_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);

    //@brief Initializes an image create info with specified properties.
    //@param [format] The format of the image.
    //@param [usageFlags] Flags describing the intended usage of the image.
    //@param [extent] The extent (width, height, depth) of the image.
    //@return VkImageCreateInfo structure initialized with the provided parameters.
    VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usageFlags, extent_3D extent);
    
    //@brief Initializes an image view create info with specified properties.
    //@param [format] The format of the image.
    //@param [image] The image to create the view for.
    //@param [aspectFlags] Flags specifying which aspect of the image to view.
    //@return VkImageViewCreateInfo structure initialized with the provided parameters.
    VkImageViewCreateInfo imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
    
    //@brief Initializes a pipeline layout create info with default values.
    //@return VkPipelineLayoutCreateInfo structure initialized with default values.
    VkPipelineLayoutCreateInfo pipeline_layout_create_info();
    
    //@brief Initializes a pipeline shader stage create info with specified properties.
    //@param [stage] The stage of the pipeline shader.
    //@param [shaderModule] The shader module containing the code.
    //@param [entry] The entry point of the shader.
    //@return VkPipelineShaderStageCreateInfo structure initialized with the provided parameters.
    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shaderModule, const char* entry = "main");
}