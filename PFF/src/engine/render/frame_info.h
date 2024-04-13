#pragma once

#include "util/pffpch.h"
#include "engine/game_objects/camera.h"
#include <vulkan/vulkan.h>

#include "engine/game_objects/game_object.h"

namespace PFF {

	struct frame_info {

		u32 frame_index;
		f64 frame_time;
		VkCommandBuffer command_buffer;
		ref<camera> camera;
		VkDescriptorSet global_descriptor_set;
		game_object::map& game_objects;
	};

}