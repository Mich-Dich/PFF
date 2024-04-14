#pragma once

#include "vk_types.h"
#include "vendor/VkBootstrap.h"


namespace PFF::render::vulkan {

	vkb::Instance create_bootstrap_instance() {

		vkb::InstanceBuilder builder;
		auto inst_ret = builder.set_app_name("PFF_GameEngine")
			.request_validation_layers(true)
			//.set_debug_callback()
			.use_default_debug_messenger()
			.set_minimum_instance_version(1, 3, 0)
			.require_api_version(1, 3, 0)
			.build();

		return inst_ret.value();
	}

	void create_instance(vkb::Instance& vkb_instance, VkDebugUtilsMessengerEXT& messanger) {


	}

}