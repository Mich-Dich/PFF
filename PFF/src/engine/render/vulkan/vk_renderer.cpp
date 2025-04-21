
#include "util/pffpch.h"

// ========== vulkan utils ============
#include "vk_types.h"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "vendor/VkBootstrap.h"
#include "vk_initializers.h"
#include "vk_instance.h"
#include "vk_image.h"
#include "vk_pipeline.h"

// ========== ImGui/ImGuizmo ============
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <ImGuizmo.h>
#include "engine/layer/imgui_layer.h"

// ========== misc ============
#include "GLFW/glfw3.h"
#include "application.h"
#include "util/system.h"
#include "util/ui/pannel_collection.h"
#include "util/io/serializer_yaml.h"
#include "util/ui/pannel_collection.h"
#include "engine/platform/pff_window.h"
#include "engine/layer/layer_stack.h"
#include "engine/layer/layer.h"
#include "engine/world/map.h"
#include "engine/world/components.h"
#include "project/script_system.h"
#include "procedural/procedural_mesh_script.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <cstdlib> // for system calls (conpieling shaders)

#if defined(PFF_PLATFORM_LINUX)
	#include <xmmintrin.h> // For SSE
	#include <emmintrin.h> // For SSE2
	#include <smmintrin.h> // For SSE4.1
#endif

#include "vk_renderer.h"

namespace PFF::render::vulkan {

#if	PFF_DEBUG
	#define COLLECT_PERFORMANCE_DATA
#endif

#ifdef COLLECT_PERFORMANCE_DATA
	#define COLLECTING_PERFORMANCE_DATA(data)			data
#else
	#define COLLECTING_PERFORMANCE_DATA(...)
#endif // COLLECT_PERFORMANCE_DATA


// #define TRY_TO_PROVIDE_SCNENE_DATA_TO_COMP


	vk_renderer vk_renderer::s_instance = vk_renderer{};
	static std::vector<std::vector<std::function<void()>>> s_resource_free_queue;
	// static std::vector<std::unordered_map<std::function<void()>, u16>> s_resource_free_queue_TEST;		// use u16 as a counter to free resources after all command buffers are free


	void deletion_queue:: setup(VkDevice device, VmaAllocator allocator) {
		
		m_dq_device = device; 
		m_dq_allocator = allocator;
	}

	void deletion_queue::cleanup() {
		
		m_dq_device = nullptr;
		m_dq_allocator = nullptr;
		
		LOG_SHUTDOWN();
	}

	void deletion_queue::push_func(std::function<void()>&& function) { m_deletors.push_back(function); }

	void deletion_queue::flush() {

#define IS_OF_TYPE(name)							entry.first == std::type_index(typeid(name))
#define USE_AS(name)								static_cast<name>(entry.second)
#define VK_DESTROY_FUNC(name)						if (IS_OF_TYPE(Vk##name))	{vkDestroy##name(m_dq_device, USE_AS(Vk##name), nullptr); }

		for (auto it = m_deletors.rbegin(); it != m_deletors.rend(); it++)
			(*it)();
		m_deletors.clear();
				
		for (const auto& entry : m_pointers) {

			VK_DESTROY_FUNC(Sampler)
			else VK_DESTROY_FUNC(CommandPool)
			else VK_DESTROY_FUNC(DescriptorPool)
			else VK_DESTROY_FUNC(Fence)
			else VK_DESTROY_FUNC(DescriptorSetLayout)
			else VK_DESTROY_FUNC(Pipeline)
			else VK_DESTROY_FUNC(PipelineLayout)

			else if (IS_OF_TYPE(ref<image>*))						{ USE_AS(ref<image>*)->reset(); }
			else if (IS_OF_TYPE(vk_buffer*))						{ GET_RENDERER.destroy_buffer(*USE_AS(vk_buffer*)); }
			else if (IS_OF_TYPE(descriptor_allocator_growable*))	{ USE_AS(descriptor_allocator_growable*)->destroy_pools(m_dq_device); }

			else if (IS_OF_TYPE(descriptor_allocator*)) {

				USE_AS(descriptor_allocator*)->clear_descriptors(m_dq_device);
				USE_AS(descriptor_allocator*)->destroy_pool(m_dq_device);
			}

			else if (IS_OF_TYPE(image*)) {

				image* loc_image = USE_AS(image*);
				vkDestroyImageView(m_dq_device, loc_image->get_image_view(), nullptr);
				vmaDestroyImage(m_dq_allocator, loc_image->get_image(), loc_image->get_allocation());
			}

			else
				LOG(Error, "Renderer deletion queue used with an unknown type [" << entry.first.name() << "]");
		}
		m_pointers.clear();
	}

	// ============================================= setup  ============================================= 

	void vk_renderer::setup(ref<pff_window> window, ref<PFF::layer_stack> layer_stack) {

		m_window = window;
		m_layer_stack = layer_stack;

		LOG_INIT();

#if defined(PFF_PLATFORM_WINDOWS)					// compile_shaders_in_dir
		const std::filesystem::path path_to_build_script = PFF::util::get_executable_path().parent_path() / "PFF_helper" / "PFF_helper.exe";
		std::string cmdArgs = "1 1 0 " + (PFF::util::get_executable_path().parent_path() / "PFF/shaders").generic_string() + " 1 ";
#elif defined(PFF_PLATFORM_LINUX)
		const std::filesystem::path path_to_build_script = PFF::util::get_executable_path().parent_path() / "PFF_helper" / "PFF_helper";
		std::string cmdArgs = "1 1 0 " + (PFF::util::get_executable_path().parent_path() / "PFF/shaders").generic_string() + " 1 ";
#endif

		ASSERT(PFF::util::run_program(path_to_build_script, cmdArgs), "called to compiled shaders for [" << cmdArgs << "]", "Failed to compile shaders using PFF_helper");			// make sure PFF_helper is build

		//make the vulkan instance, with basic debug features
		vkb::InstanceBuilder builder;
		auto inst_ret = builder.set_app_name("PFF_GameEngine")
			.request_validation_layers(true)
			.use_default_debug_messenger()
			.require_api_version(1, 3, 0)
			.build();

		vkb::Instance vkb_inst = inst_ret.value();

		//grab the instance 
		m_instance = vkb_inst.instance;
		m_debug_messenger = vkb_inst.debug_messenger;

		m_window->create_vulkan_surface(m_instance, &m_surface);

		VkPhysicalDeviceFeatures feature{};							//vulkan features
		feature.fillModeNonSolid = true;

		VkPhysicalDeviceVulkan12Features features_1_2{};			//vulkan 1.2 features
		features_1_2.bufferDeviceAddress = true;
		features_1_2.descriptorIndexing = true;

		VkPhysicalDeviceVulkan13Features features_1_3{};			//vulkan 1.3 features
		features_1_3.dynamicRendering = true;
		features_1_3.synchronization2 = true;

		// select a gpu that can write to GLFW-surface and supports vulkan 1.3 with the correct features
		vkb::PhysicalDeviceSelector selector{ vkb_inst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 3)
			.set_required_features(feature)
			.set_required_features_12(features_1_2)
			.set_required_features_13(features_1_3)
			.set_surface(m_surface)
			.select()
			.value();

		//create the final vulkan device
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };
		vkb::Device vkbDevice = deviceBuilder.build().value();

		// Get the VkDevice handle used in the rest of a vulkan application
		m_device = vkbDevice.device;
		m_chosenGPU = physicalDevice.physical_device;

		// use vkbootstrap to get a Graphics queue
		m_graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

		// initialize the memory allocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_chosenGPU;
		allocatorInfo.device = m_device;
		allocatorInfo.instance = m_instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&allocatorInfo, &m_allocator);

		s_resource_free_queue.resize(FRAME_COUNT);

		// Setup deletion queues
		m_deletion_queue.setup(m_device, m_allocator);
		for (int i = 0; i < FRAME_COUNT; i++)
			m_frames[i].del_queue.setup(m_device, m_allocator);

		init_commands();
		init_swapchain();
		init_sync_structures();
		init_descriptors();
		init_pipelines();
		init_default_data();
		init_grid_pipeline();

		m_is_initialized = true;
	}


	void vk_renderer::shutdown() {
		
		if (!m_is_initialized)
			return;

		vkDeviceWaitIdle(m_device);
		m_state = system_state::inactive;

		serialize(PFF::serializer::option::save_to_file);

		for (auto frame : m_frames) {

			vkDestroyCommandPool(m_device, frame.command_pool, nullptr);
			vkDestroyFence(m_device, frame.render_fence, nullptr);
			vkDestroySemaphore(m_device, frame.render_semaphore, nullptr);
			vkDestroySemaphore(m_device, frame.swapchain_semaphore, nullptr);

			frame.frame_descriptors.clear_pools(m_device);
			frame.del_queue.flush();
		}

		m_deletion_queue.flush();
		m_deletion_queue.cleanup();

		vkDestroyPipelineLayout(m_device, m_grid_pipeline_layout, nullptr);
		vkDestroyPipeline(m_device, m_grid_pipeline, nullptr);
		
		destroy_swapchain();
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vmaDestroyAllocator(m_allocator);
		vkDestroyDevice(m_device, nullptr);
		vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
		vkDestroyInstance(m_instance, nullptr);
		
		m_window.reset();
		m_layer_stack.reset();

		LOG_SHUTDOWN();
	}


	void vk_renderer::resource_free() {

		vkDeviceWaitIdle(m_device);

		// Free resources in queue
		for (auto& queue : s_resource_free_queue) {
			for (auto& func : queue)
				func();
		}
		s_resource_free_queue.clear();

	}

	// =======================================================================================================================================================================================
	// PUBLIC FUNCTIONS
	// =======================================================================================================================================================================================

	void vk_renderer::imgui_init() {
		
		if (m_imgui_initalized)
			return;

		// create descriptor pool for IMGUI (the size of the pool is very oversize, but it's copied from imgui demo itself)
		VkDescriptorPoolSize pool_sizes[] = { 
			{ VK_DESCRIPTOR_TYPE_SAMPLER,					1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,		1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,		1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			1000 },
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = static_cast<u32>(std::size(pool_sizes));
		pool_info.pPoolSizes = pool_sizes;
		VK_CHECK_S(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_imgui_desc_pool));

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
		// Viewport enable flags (require both ImGuiBackendFlags_PlatformHasViewports + ImGuiBackendFlags_RendererHasViewports set by the respective backends)
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows	
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		io.IniFilename = NULL;										// save data in custom file

		// check for imgui ini-file or copy from default file
		if (!std::filesystem::exists(UI::ini_file_location)) {

			std::ofstream file(UI::ini_file_location);
			ASSERT(file.is_open(), "", "Failed to open [" << UI::ini_file_location << "] default: [" << io.IniFilename << "]");

			std::ifstream default_config_file(PFF::util::get_executable_path() / "defaults" / "imgui.ini");
			ASSERT(default_config_file.is_open(), "", "Failed to open [default_config_file]");

			LOG(Trace, "[imgui_config.ini] is empty. Copying data from default file");
			file << default_config_file.rdbuf();

			default_config_file.close();
			file.close();
		}
		ImGui::LoadIniSettingsFromDisk(UI::ini_file_location.string().c_str());


		ASSERT(ImGui_ImplGlfw_InitForVulkan(m_window->get_window(), true), "", "Failed to initalize imgui -> init GLFW for Vulkan");

		// this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_instance;
		init_info.PhysicalDevice = m_chosenGPU;
		init_info.Device = m_device;
		init_info.Queue = m_graphics_queue;
		init_info.DescriptorPool = m_imgui_desc_pool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.UseDynamicRendering = true;
		init_info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &m_swapchain_image_format;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ASSERT(ImGui_ImplVulkan_Init(&init_info), "", "Failed to initalize ImGui-Vulkan");

		// execute a gpu command to upload imgui font textures
		// immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

		// serializer::yaml(config::get_filepath_from_configtype(config::file::editor), "UI", serializer::option::load_from_file)
		// 	.entry(KEY_VALUE(UI::THEME::main_color))
		// 	.entry(KEY_VALUE(UI::THEME::UI_theme));
		// UI::THEME::update_UI_theme();


		//void vk_renderer::createTextureSampler() {  ===============================================================
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VK_CHECK_S(vkCreateSampler(m_device, &samplerInfo, nullptr, &m_texture_sampler));
		// }  ===========================================================================================

		m_draw_image.generate_descriptor_set(m_texture_sampler, VK_IMAGE_LAYOUT_GENERAL);

		m_deletion_queue.push_pointer(m_texture_sampler);

		m_imgui_initalized = true;
	}


	void vk_renderer::imgui_create_fonts() { immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); }); }


	void vk_renderer::imgui_destroy_fonts() { immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_DestroyFontsTexture(); }); }


	void vk_renderer::imgui_shutdown() { 

		ImGui::SaveIniSettingsToDisk(UI::ini_file_location.string().c_str());

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		vkDestroyDescriptorPool(m_device, m_imgui_desc_pool, nullptr);

		m_imgui_initalized = false;
	}

	
#define TRY_TO_PROVIDE_SCNENE_DATA_TO_COMP
// #ifdef TRY_TO_PROVIDE_SCNENE_DATA_TO_COMP
		// VkDescriptorSet m_global_descriptor;
// #endif

	void vk_renderer::draw_frame(f32 delta_time) {

		if (m_state != system_state::active)
			return;

#ifdef COLLECT_PERFORMANCE_DATA
		m_renderer_metrik.reset();
		PFF::stopwatch loc_stopwatch(&m_renderer_metrik.renderer_draw_time[m_renderer_metrik.current_index]);
		{
			PFF::stopwatch loc_stopwatch(&m_renderer_metrik.waiting_idle_time[m_renderer_metrik.current_index]);
#endif // COLLECT_PERFORMANCE_DATA

		VK_CHECK_S(vkWaitForFences(m_device, 1, &get_current_frame().render_fence, true, 3000000000));		//wait until the gpu has finished rendering the last frame. Timeout of 3 second

#ifdef COLLECT_PERFORMANCE_DATA
		}
#endif // COLLECT_PERFORMANCE_DATA

		VK_CHECK_S(vkResetFences(m_device, 1, &get_current_frame().render_fence));


		// ============================================================ DEV-ONLY ============================================================
		// f32 rotation_speed = glm::radians(25.0f); // 25 degrees per second
		// f32 angle = rotation_speed * delta_time;
		// glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));				// rotation matrix around the Z-axis
		// glm::vec4 rotated_dir = rotation * m_scene_data.sunlight_direction;
		// m_scene_data.sunlight_direction = glm::vec4(rotated_dir.x, rotated_dir.y, rotated_dir.z, m_scene_data.sunlight_direction.w);
		// ============================================================ DEV-ONLY ============================================================



		// Free resources 
		{
			for (auto& func : s_resource_free_queue[(m_frame_number) % FRAME_COUNT]) {
				LOG(Info, "Executing free QUEUE");
				func();
			}
			s_resource_free_queue[(m_frame_number) % FRAME_COUNT].clear();

			get_current_frame().del_queue.flush();
			get_current_frame().frame_descriptors.clear_pools(m_device);
		}

		u32 swapchain_image_index;
		VkResult e = vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, get_current_frame().swapchain_semaphore, nullptr, &swapchain_image_index);
		if (e == VK_ERROR_OUT_OF_DATE_KHR) {
			m_resize_nedded = true;
			resize_swapchain();
			return;					// this skips a frame, but it fine
		}

		VkCommandBuffer cmd = get_current_frame().main_command_buffer;
		VK_CHECK_S(vkResetCommandBuffer(cmd, 0));

		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		if (m_imgui_initalized && !m_render_swapchain) {

			m_draw_extent.width = math::min(m_draw_image.get_width(), m_imugi_viewport_size.x) * (u32)m_render_scale;
			m_draw_extent.height = math::min(m_draw_image.get_height(), m_imugi_viewport_size.y) * (u32)m_render_scale;
		} else {

			m_draw_extent.width = math::min(m_swapchain_extent.width, m_draw_image.get_width()) * (u32)m_render_scale;
			m_draw_extent.height = math::min(m_swapchain_extent.height, m_draw_image.get_height()) * (u32)m_render_scale;
		}

#ifdef TRY_TO_PROVIDE_SCNENE_DATA_TO_COMP
		// ============================================================ calc scene data ============================================================
		m_scene_data.view = m_active_camera->get_view();
		m_scene_data.proj = glm::perspective(glm::radians(m_active_camera->get_perspective_fov_y()), (float)m_draw_extent.width / (float)m_draw_extent.height, 100000.f, 0.1f);
		m_active_camera->force_set_projection_matrix(m_scene_data.proj);
		m_scene_data.proj[1][1] *= -1;				// invert the Y direction on projection matrix
		m_scene_data.proj_view = m_scene_data.proj * m_scene_data.view;

		// allocate a new uniform buffer for the scene data
		vk_buffer gpuSceneDataBuffer = create_buffer(sizeof(render::GPU_scene_data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		get_current_frame().del_queue.push_func([this, gpuSceneDataBuffer]() { destroy_buffer(gpuSceneDataBuffer); });

		//write the buffer
		render::GPU_scene_data* scene_uniform_data = (render::GPU_scene_data*)gpuSceneDataBuffer.allocation->GetMappedData();
		*scene_uniform_data = m_scene_data;

		//create a descriptor set that binds that buffer and update it
		m_global_descriptor = get_current_frame().frame_descriptors.allocate(m_device, m_gpu_scene_data_descriptor_layout);
		descriptor_writer writer;
		writer.write_buffer(0, gpuSceneDataBuffer.buffer, sizeof(render::GPU_scene_data), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.update_set(m_device, m_global_descriptor);
#endif // TRY_TO_PROVIDE_SCNENE_DATA_TO_COMP

		VK_CHECK_S(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		util::transition_image(cmd, m_draw_image.get_image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		draw_internal(cmd);

		util::transition_image(cmd, m_draw_image.get_image(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		util::transition_image(cmd, m_depth_image.get_image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

		draw_geometry(cmd);

		//draw_debug(cmd);

		if (m_render_swapchain) {

			//transition the draw image and the swapchain image into their correct transfer layouts
			util::transition_image(cmd, m_draw_image.get_image(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			util::copy_image_to_image(cmd, m_draw_image.get_image(), m_swapchain_images[swapchain_image_index], m_draw_extent, m_swapchain_extent);						// copy from draw_image into swapchain

		} else 
			util::transition_image(cmd, m_draw_image.get_image(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);


		// =========================================================== draw imgui ===========================================================
		if (m_imgui_initalized) {
		
			ImGui::SetCurrentContext(application::get().get_imgui_layer()->get_context());

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGuizmo::SetImGuiContext(application::get().get_imgui_layer()->get_context());
			ImGuizmo::BeginFrame();

			for (layer* layer : *m_layer_stack) 
				layer->on_imgui_render();

			ImGui::EndFrame();
			ImGui::Render();

			// update other platform windows
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);

			if (m_render_swapchain) {
				
				util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				draw_imgui(cmd, m_swapchain_image_views[swapchain_image_index]);		//draw imgui into the swapchain image
				util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

			} else {

				util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				draw_imgui(cmd, m_swapchain_image_views[swapchain_image_index]);		//draw imgui into the swapchain image
				util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}
		}
		// =========================================================== draw imgui ===========================================================

		else
			util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
				
		VK_CHECK_S(vkEndCommandBuffer(cmd));

		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSemaphoreSubmitInfo waitInfo = init::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame().swapchain_semaphore);
		VkSemaphoreSubmitInfo signalInfo = init::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame().render_semaphore);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, &signalInfo, &waitInfo);
		VK_CHECK_S(vkQueueSubmit2(m_graphics_queue, 1, &submit, get_current_frame().render_fence));		// [m_render_fence] will now block until the graphic commands finish execution

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_swapchain;
		presentInfo.swapchainCount = 1;
		presentInfo.pWaitSemaphores = &get_current_frame().render_semaphore;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pImageIndices = &swapchain_image_index;
		VkResult queue_present_result = vkQueuePresentKHR(m_graphics_queue, &presentInfo);
		if (queue_present_result == VK_ERROR_OUT_OF_DATE_KHR) {
			m_resize_nedded = true;
			resize_swapchain();
		}

		if (m_frame_number > 184467440737095)
			m_frame_number = m_frame_number % FRAME_COUNT;

		m_frame_number++;
	}


	void vk_renderer::refresh(f32 delta_time) {
	
		//LOG(Debug, "Refreching renderer");
		resize_swapchain();
		draw_frame(delta_time);
	}


	void vk_renderer::set_size(u32 width, u32 height) {}


	void vk_renderer::wait_idle() { vkDeviceWaitIdle(m_device); }


	void vk_renderer::immediate_submit(std::function<void()>&& function) {

		VK_CHECK_S(vkResetFences(m_device, 1, &m_immFence));
		VK_CHECK_S(vkResetCommandBuffer(m_immCommandBuffer, 0));

		VkCommandBuffer cmd = m_immCommandBuffer;
		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK_S(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		function();

		VK_CHECK_S(vkEndCommandBuffer(cmd));
		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, nullptr, nullptr);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK_S(vkQueueSubmit2(m_graphics_queue, 1, &submit, m_immFence));
		VK_CHECK_S(vkWaitForFences(m_device, 1, &m_immFence, true, 9999999999));
	}


	void vk_renderer::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {

		VK_CHECK_S(vkResetFences(m_device, 1, &m_immFence));
		VK_CHECK_S(vkResetCommandBuffer(m_immCommandBuffer, 0));

		VkCommandBuffer cmd = m_immCommandBuffer;
		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK_S(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		function(cmd);

		VK_CHECK_S(vkEndCommandBuffer(cmd));
		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, nullptr, nullptr);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK_S(vkQueueSubmit2(m_graphics_queue, 1, &submit, m_immFence));
		VK_CHECK_S(vkWaitForFences(m_device, 1, &m_immFence, true, 9999999999));
	}


	void vk_renderer::submit_resource_free(std::function<void()>&& func) {

		if (m_state == system_state::active)
			s_resource_free_queue[m_frame_number % FRAME_COUNT].emplace_back(func);
		else
			func();		// If renderer suspended/inactive, resource_free should always be possible
	}

	// =======================================================================================================================================================================================
	// PRIVATE FUNCTIONS
	// =======================================================================================================================================================================================

	// ================================================================================ INIT FUNCTION ================================================================================

	void vk_renderer::init_default_data() {

		//serialize(PFF::serializer::option::load_from_file);

		//3 default textures, white, grey, black. 1 pixel each
		u32 white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
		m_white_image = create_ref<image>((void*)&white, 1, 1, image_format::RGBA);

		u32 gray = glm::packUnorm4x8(glm::vec4(0.6f, 0.6f, 0.6f, 0.f));
		m_grey_image = create_ref<image>((void*)&gray, 1, 1, image_format::RGBA);

		u32 black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
		m_black_image = create_ref<image>((void*)&black, 1, 1, image_format::RGBA);

		//checkerboard image
		const int EDGE_LENGTH = 2;
		u32 color = glm::packUnorm4x8(glm::vec4(0.8f, 0.8f, 0.8f, 1));
		std::array<u32, EDGE_LENGTH * EDGE_LENGTH > pixels; //for checkerboard texture
		for (int x = 0; x < EDGE_LENGTH; x++)
			for (int y = 0; y < EDGE_LENGTH; y++)
				pixels[y * EDGE_LENGTH + x] = ((x % EDGE_LENGTH) ^ (y % EDGE_LENGTH)) ? gray : color;
		m_error_checkerboard_image = create_ref<image>(pixels.data(), EDGE_LENGTH, EDGE_LENGTH, image_format::RGBA);

		VkSamplerCreateInfo sampler{};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	
		sampler.magFilter = VK_FILTER_NEAREST;
		sampler.minFilter = VK_FILTER_NEAREST;
		vkCreateSampler(m_device, &sampler, nullptr, &m_default_sampler_nearest);

		sampler.magFilter = VK_FILTER_LINEAR;
		sampler.minFilter = VK_FILTER_LINEAR;
		vkCreateSampler(m_device, &sampler, nullptr, &m_default_sampler_linear);

		m_deletion_queue.push_pointer(&m_white_image);
		m_deletion_queue.push_pointer(&m_black_image);
		m_deletion_queue.push_pointer(&m_grey_image);
		m_deletion_queue.push_pointer(&m_error_checkerboard_image);
		m_deletion_queue.push_pointer(m_default_sampler_linear);
		m_deletion_queue.push_pointer(m_default_sampler_nearest);

		// =========================================================== DEFAULT SCENE DATA =========================================================== 

		m_scene_data.sunlight_color = glm::vec4(1.f, 1.f, 1.f, 0.01f);
		m_scene_data.ambient_color = glm::vec4(1.f, 1.f, 1.f, 0.001f);
		m_scene_data.sunlight_direction = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// =========================================================== DEFAULT MATERIAL =========================================================== 

		//set the uniform buffer for the material data
		m_material_constant = create_buffer(sizeof(material::material_constants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		//write the buffer
		material::material_constants* scene_uniform_data = (material::material_constants*)m_material_constant.allocation->GetMappedData();
		scene_uniform_data->color_factors = glm::vec4{ 1, 1, 1, 1 };
		scene_uniform_data->metal_rough_factors = glm::vec4{ 1, 0.5, 0, 0 };

		m_deletion_queue.push_func([this]() { destroy_buffer(m_material_constant); });

		material::material_resources material_resources;						//default the material textures
		material_resources.color_image = m_error_checkerboard_image;
		material_resources.color_sampler = m_default_sampler_nearest;
		material_resources.metal_rough_image = m_error_checkerboard_image;
		material_resources.metal_rough_sampler = m_default_sampler_nearest;
		material_resources.data_buffer = m_material_constant.buffer;
		material_resources.data_buffer_offset = 0;
		m_default_material = create_ref<material_instance>(m_metal_rough_material.create_instance(material_pass::main_color, material_resources));
		
#ifdef PFF_RENDERER_DEBUG_CAPABILITY

		material::material_resources debug_lines_material_resources;						//debug material for lines
		debug_lines_material_resources.color_image = m_error_checkerboard_image;
		debug_lines_material_resources.color_sampler = m_default_sampler_nearest;
		debug_lines_material_resources.metal_rough_image = m_error_checkerboard_image;
		debug_lines_material_resources.metal_rough_sampler = m_default_sampler_nearest;
		debug_lines_material_resources.data_buffer = m_material_constant.buffer;
		debug_lines_material_resources.data_buffer_offset = 0;
		m_debug_lines_material_inst = m_debug_lines_material.create_instance(material_pass::main_color, debug_lines_material_resources);

#endif // PFF_RENDERER_DEBUG_CAPABILITY
	 
	}


	void vk_renderer::serialize(const PFF::serializer::option option) {

		PFF::serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::engine), "renderer_background_effect", option)
			.entry("current_background_effect", m_current_background_effect)
			.vector(KEY_VALUE(m_background_effects), [this](serializer::yaml& yaml, const u64 x) {
				yaml.entry(KEY_VALUE(m_background_effects[x].name))
				.entry("data_0", m_background_effects[x].data.data1)
				.entry("data_1", m_background_effects[x].data.data2)
				.entry("data_2", m_background_effects[x].data.data3)
				.entry("data_3", m_background_effects[x].data.data4);
			});
	}


	void vk_renderer::init_swapchain() {

		create_swapchain(m_window->get_width(), m_window->get_height());

		//draw image size will match the window
		extent_3D drawImageExtent = {
			m_window->get_width(),
			m_window->get_height(),
			1 
		};
		m_window->get_monitor_size((int*)&drawImageExtent.width, (int*)&drawImageExtent.height);
		LOG(Trace, "render image extend: " << drawImageExtent.width << "/" << drawImageExtent.height);

		VmaAllocationCreateInfo image_alloc_CI = {};
		image_alloc_CI.usage = VMA_MEMORY_USAGE_GPU_ONLY;												// for the m_draw_image & m_depth_image, allocate it from GPU local memory
		image_alloc_CI.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);		// ensure image is only on GPU (only GPU side VRAM has this flag)

		// =========================================== create draw_image ===========================================

		m_draw_image.set_image_format(VK_FORMAT_R16G16B16A16_SFLOAT);
		m_draw_image.set_image_extent(drawImageExtent);

		VkImageUsageFlags draw_image_usages{};
		draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_STORAGE_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_SAMPLED_BIT;
		//draw_image_usages |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		VkImageCreateInfo image_CI = init::image_create_info(m_draw_image.get_image_format(), draw_image_usages, drawImageExtent);

		vmaCreateImage(m_allocator, &image_CI, &image_alloc_CI, m_draw_image.get_image_pointer(), m_draw_image.get_allocation_pointer(), nullptr);						// allocate and create the image
		VkImageViewCreateInfo view_CI = init::imageview_create_info(m_draw_image.get_image_format(), m_draw_image.get_image(), VK_IMAGE_ASPECT_COLOR_BIT);	// build a image-view for the draw image to use for rendering
		VK_CHECK_S(vkCreateImageView(m_device, &view_CI, nullptr, m_draw_image.get_image_view_pointer()));

		// =========================================== create depth_image ===========================================
		
		m_depth_image.set_image_format(VK_FORMAT_D32_SFLOAT);
		m_depth_image.set_image_extent(drawImageExtent);

		VkImageUsageFlags depthImageUsages{};
		depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		VkImageCreateInfo dimg_info = init::image_create_info(m_depth_image.get_image_format(), depthImageUsages, drawImageExtent);
		
		vmaCreateImage(m_allocator, &dimg_info, &image_alloc_CI, m_depth_image.get_image_pointer(), m_depth_image.get_allocation_pointer(), nullptr);
		VkImageViewCreateInfo dview_info = init::imageview_create_info(m_depth_image.get_image_format(), m_depth_image.get_image(), VK_IMAGE_ASPECT_DEPTH_BIT);
		VK_CHECK_S(vkCreateImageView(m_device, &dview_info, nullptr, m_depth_image.get_image_view_pointer()));

		m_deletion_queue.push_pointer(&m_draw_image);
		m_deletion_queue.push_pointer(&m_depth_image);
	}


	void vk_renderer::init_commands() {

		// create a command pool for commands submitted to the graphics queue.
		// also allow the pool to allow for resetting of individual command buffers
		VkCommandPoolCreateInfo command_pool_CI = init::command_pool_create_info(m_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		for (int i = 0; i < FRAME_COUNT; i++) {

			VK_CHECK_S(vkCreateCommandPool(m_device, &command_pool_CI, nullptr, &m_frames[i].command_pool));
			VkCommandBufferAllocateInfo command_alloc_I = init::command_buffer_allocate_info(m_frames[i].command_pool, 1);			// allocate default command buffer used for rendering
			VK_CHECK_S(vkAllocateCommandBuffers(m_device, &command_alloc_I, &m_frames[i].main_command_buffer));
		}

		VK_CHECK_S(vkCreateCommandPool(m_device, &command_pool_CI, nullptr, &m_immCommandPool));
		VkCommandBufferAllocateInfo cmdAllocInfo = init::command_buffer_allocate_info(m_immCommandPool, 1);
		VK_CHECK_S(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_immCommandBuffer));

		m_deletion_queue.push_pointer(m_immCommandPool);
	}


	void vk_renderer::init_sync_structures() {

		// one fence to control when the GPU finished rendering the frame,
		// 2 semaphores to syncronize rendering with swapchain
		VkFenceCreateInfo fence_CI = init::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);		// init signaled to avoid thread blocking
		VkSemaphoreCreateInfo semaphore_CI = init::semaphore_create_info();

		for (int i = 0; i < FRAME_COUNT; i++) {

			VK_CHECK_S(vkCreateFence(m_device, &fence_CI, nullptr, &m_frames[i].render_fence));
			VK_CHECK_S(vkCreateSemaphore(m_device, &semaphore_CI, nullptr, &m_frames[i].swapchain_semaphore));
			VK_CHECK_S(vkCreateSemaphore(m_device, &semaphore_CI, nullptr, &m_frames[i].render_semaphore));
		}

		VK_CHECK_S(vkCreateFence(m_device, &fence_CI, nullptr, &m_immFence));
		m_deletion_queue.push_pointer(m_immFence);
	}


	void vk_renderer::init_descriptors() {

		//create a descriptor pool that will hold 10 sets with 1 image each
		std::vector<descriptor_allocator_growable::pool_size_ratio> sizes = {
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
		};
		m_global_descriptor_allocator.init(m_device, 10, sizes);

		{
			//make the descriptor set layout for our compute draw
			descriptor_layout_builder builder;
			builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			builder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_draw_image_descriptor_layout = builder.build(m_device, VK_SHADER_STAGE_COMPUTE_BIT);
			m_draw_image_descriptors = m_global_descriptor_allocator.allocate(m_device, m_draw_image_descriptor_layout);
		}

		{
			descriptor_writer writer;
			writer.write_image(0, m_draw_image.get_image_view(), VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			writer.update_set(m_device, m_draw_image_descriptors);
		}
		
		m_deletion_queue.push_pointer(m_draw_image_descriptor_layout);
		m_deletion_queue.push_pointer(m_single_image_descriptor_layout);
		m_deletion_queue.push_pointer(m_gpu_scene_data_descriptor_layout);
		m_deletion_queue.push_func([&] { m_global_descriptor_allocator.destroy_pools(m_device); });
		
		{
			descriptor_layout_builder builder;
			builder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_single_image_descriptor_layout = builder.build(m_device, VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		// =========================================== init frame descriptor pool ===========================================

		for (int i = 0; i < FRAME_COUNT; i++) {

			// create a descriptor pool
			std::vector<descriptor_allocator_growable::pool_size_ratio> frame_sizes = {
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
			};

			m_frames[i].frame_descriptors = descriptor_allocator_growable{};
			m_frames[i].frame_descriptors.init(m_device, 1000, frame_sizes);

			m_deletion_queue.push_func([this, i]() { m_frames[i].frame_descriptors.destroy_pools(m_device); });
		}

		{
			descriptor_layout_builder builder;
			builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			m_gpu_scene_data_descriptor_layout = builder.build(m_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

			m_deletion_queue.push_pointer(m_gpu_scene_data_descriptor_layout);
		}
	}


	void vk_renderer::init_grid_pipeline() {
	
		VkShaderModule gridVertexShader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/world_grid.vert.spv", m_device, &gridVertexShader), "Loaded grid vertex shader", "Error loading grid vertex shader");
	
		VkShaderModule gridFragmentShader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/world_grid.frag.spv", m_device, &gridFragmentShader), "Loaded grid fragment shader", "Error loading grid fragment shader");
	
		// Descriptor set layouts
		VkDescriptorSetLayout layouts[] = {
			m_gpu_scene_data_descriptor_layout,  // Set 0: Scene data
			// Add other layouts if needed
		};

		// Create a pipeline layout
		VkPipelineLayoutCreateInfo pipeline_layout_info = init::pipeline_layout_create_info();
		pipeline_layout_info.setLayoutCount = 1;
		pipeline_layout_info.pSetLayouts = layouts;
		pipeline_layout_info.pushConstantRangeCount = 0;
		VK_CHECK_S(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_grid_pipeline_layout));

		// Create the graphics pipeline
		pipeline_builder gridPipelineBuilder;
		gridPipelineBuilder.set_shaders(gridVertexShader, gridFragmentShader)
			.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.set_polygon_mode(VK_POLYGON_MODE_FILL)
			.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
			.set_multisampling_none()
			.set_color_attachment_format(m_draw_image.get_image_format())
			.set_depth_format(m_depth_image.get_image_format())
			.set_pipeline_layout(m_grid_pipeline_layout);
	
		m_grid_pipeline = gridPipelineBuilder.build(m_device);
	
		// Clean up shader modules
		vkDestroyShaderModule(m_device, gridVertexShader, nullptr);
		vkDestroyShaderModule(m_device, gridFragmentShader, nullptr);
	}

	// ================================================================================ INIT PIPELINES ================================================================================

	void vk_renderer::init_pipelines() {

		// COMPUTE PIPELINES	
		init_pipelines_background();

		// GRAPHICS PIPELINES
		init_pipeline_mesh();

		m_metal_rough_material.build_pipelines();
		m_deletion_queue.push_func([&] { m_metal_rough_material.release_resources(); });

#ifdef PFF_RENDERER_DEBUG_CAPABILITY

		build_debug_lines_pipelines(m_debug_lines_material);
		m_deletion_queue.push_func([&] { m_debug_lines_material.release_resources(); });

#endif // PFF_RENDERER_DEBUG_CAPABILITY
	}


	void vk_renderer::init_pipelines_background() {

		VkPushConstantRange pushConstant{};
		pushConstant.offset = 0;
		pushConstant.size = sizeof(render::compute_push_constants);
		pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkPipelineLayoutCreateInfo computeLayout_CI{};
		computeLayout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout_CI.pNext = nullptr;
		computeLayout_CI.pSetLayouts = &m_draw_image_descriptor_layout;
		computeLayout_CI.setLayoutCount = 1;
		computeLayout_CI.pPushConstantRanges = &pushConstant;
		computeLayout_CI.pushConstantRangeCount = 1;
		VK_CHECK_S(vkCreatePipelineLayout(m_device, &computeLayout_CI, nullptr, &m_gradient_pipeline_layout));

		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		// TIP: giving it the name of the function we want the shader to use => main().
		//		It is possible to store multiple compute shader variants on the same shader file, by using different entry point functions and then setting them up here.
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo compute_pipeline_CI{};
		compute_pipeline_CI.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		compute_pipeline_CI.pNext = nullptr;
		compute_pipeline_CI.layout = m_gradient_pipeline_layout;
		compute_pipeline_CI.stage = stageinfo;

		// ====================================================== Add pipeline [grid] ====================================================== 

		VkShaderModule grid_shader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/gradient.comp.spv", m_device, &grid_shader), "", "Error when building the compute shader");
		compute_pipeline_CI.stage.module = grid_shader;		//change the shader module only

		render::compute_effect grid{};
		grid.layout = m_gradient_pipeline_layout;
		grid.name = "grid";

		VK_CHECK_S(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_CI, nullptr, &grid.pipeline));
		m_background_effects.emplace_back(grid);
		vkDestroyShaderModule(m_device, grid_shader, nullptr);

		// ====================================================== Add pipeline [gradient] ====================================================== 		

		VkShaderModule gradient_shader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/gradient_color.comp.spv", m_device, &gradient_shader), "", "Error when building the compute shader");
		compute_pipeline_CI.stage.module = gradient_shader;	//change the shader module only

		render::compute_effect gradient{};
		gradient.layout = m_gradient_pipeline_layout;
		gradient.name = "gradient";
		// --------------- default gradient color --------------- 
		gradient.data.data1 = glm::vec4(0.24f, 0.24f, 0.24f, 1.f);
		gradient.data.data2 = glm::vec4(0.24f, 0.24f, 0.24f, 1.f);

		VK_CHECK_S(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_CI, nullptr, &gradient.pipeline));
		m_background_effects.emplace_back(gradient);
		vkDestroyShaderModule(m_device, gradient_shader, nullptr);

		// ====================================================== Add pipeline [sky] ====================================================== 

		VkShaderModule sky_shader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/sky.comp.spv", m_device, &sky_shader), "", "Error when building the compute shader");
		compute_pipeline_CI.stage.module = sky_shader;		//change the shader module only

		render::compute_effect sky{};
		sky.layout = m_gradient_pipeline_layout;
		sky.name = "sky";
		// --------------- default sky parameters --------------- 
		sky.data.data1 = glm::vec4(.1f, .2f, .4f, .99f);

		//change the shader module only
		VK_CHECK_S(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_CI, nullptr, &sky.pipeline));
		m_background_effects.emplace_back(sky);
		vkDestroyShaderModule(m_device, sky_shader, nullptr);

		// ====================================================== Add pipeline [gradient] ====================================================== 		


		VkPushConstantRange skybox_push_const{};
		skybox_push_const.offset = 0;
		skybox_push_const.size = sizeof(render::compute_push_constants_dynamic_skybox);
		skybox_push_const.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkPipelineLayoutCreateInfo skybox_compute_lyout_CI{};
		skybox_compute_lyout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		skybox_compute_lyout_CI.pNext = nullptr;
		skybox_compute_lyout_CI.pSetLayouts = &m_draw_image_descriptor_layout;
		skybox_compute_lyout_CI.setLayoutCount = 1;
		skybox_compute_lyout_CI.pPushConstantRanges = &skybox_push_const;
		skybox_compute_lyout_CI.pushConstantRangeCount = 1;
		VK_CHECK_S(vkCreatePipelineLayout(m_device, &skybox_compute_lyout_CI, nullptr, &m_skybox_pipeline_layout));

		compute_pipeline_CI.layout = m_skybox_pipeline_layout;

		VkShaderModule skybox_shader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/dynamic_skybox.comp.spv", m_device, &skybox_shader), "", "Error when building the compute shader [dynamic_skybox]");
		compute_pipeline_CI.stage.module = skybox_shader;		//change the shader module only

		//change the shader module only
		VK_CHECK_S(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_CI, nullptr, &m_skybox_pipeline));
		vkDestroyShaderModule(m_device, skybox_shader, nullptr);


		m_deletion_queue.push_func([&]() {

			vkDestroyPipelineLayout(m_device, m_skybox_pipeline_layout, nullptr);
			vkDestroyPipeline(m_device, m_skybox_pipeline, nullptr);
			
			vkDestroyPipelineLayout(m_device, m_gradient_pipeline_layout, nullptr);
			for (u64 x = 0; x < m_background_effects.size(); x++)
				vkDestroyPipeline(m_device, m_background_effects[x].pipeline, nullptr);
			});
	}


	void vk_renderer::init_pipeline_mesh() {
	
#if 1	// Try new initalization with new shader
		VkShaderModule mesh_frag_shader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/tex_image.frag.spv", m_device, &mesh_frag_shader), "", "Error when building the fragment shader");

		VkShaderModule mesh_vertex_shader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/colored_triangle_mesh.vert.spv", m_device, &mesh_vertex_shader), "", "Error when building the vertex shader");

		VkPushConstantRange bufferRange{};
		bufferRange.offset = 0;
		bufferRange.size = sizeof(GPU_draw_push_constants);
		bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipeline_layout_info = init::pipeline_layout_create_info();
		pipeline_layout_info.pPushConstantRanges = &bufferRange;
		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pSetLayouts = &m_single_image_descriptor_layout;
		pipeline_layout_info.setLayoutCount = 1;
		VK_CHECK_S(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_mesh_pipeline_layout));
#else
		VkShaderModule mesh_frag_shader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/colored_triangle.frag.spv", m_device, &mesh_frag_shader), "", "Error when building the triangle fragment shader module");

		VkShaderModule mesh_vertex_shader;
		ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/colored_triangle_mesh.vert.spv", m_device, &mesh_vertex_shader), "", "Error when building the triangle vertex shader module");

		VkPushConstantRange push_constant_range{};
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(GPU_draw_push_constants);
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipeline_layout_info = init::pipeline_layout_create_info();
		pipeline_layout_info.pPushConstantRanges = &push_constant_range;
		pipeline_layout_info.pushConstantRangeCount = 1;
		VK_CHECK_S(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_mesh_pipeline_layout));
#endif

		m_mesh_pipeline = pipeline_builder()
			.set_pipeline_layout(m_mesh_pipeline_layout)				// use the triangle layout we created
			.set_shaders(mesh_vertex_shader, mesh_frag_shader)			// connecting the vertex and pixel shaders to the pipeline
			.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)	// it will draw triangles
			.set_polygon_mode(VK_POLYGON_MODE_FILL)						// filled triangles
			.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)	// no backface culling
			.set_multisampling_none()									// no multisampling
			//.disable_blending()										// no blending
			.enable_blending_additive()
			//.disable_depthtest()										
			.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL)
			.set_color_attachment_format(m_draw_image.get_image_format())		// connect the image format we will draw into, from draw image
			.set_depth_format(m_depth_image.get_image_format())
			.build(m_device);

		//clean structures
		vkDestroyShaderModule(m_device, mesh_frag_shader, nullptr);
		vkDestroyShaderModule(m_device, mesh_vertex_shader, nullptr);

		m_deletion_queue.push_func([&]() {

			vkDestroyDescriptorSetLayout(m_device, m_single_image_descriptor_layout, nullptr);
			vkDestroyPipelineLayout(m_device, m_mesh_pipeline_layout, nullptr);
			vkDestroyPipeline(m_device, m_mesh_pipeline, nullptr);
		});
	}

	// ================================================================================== PIPELINES ==================================================================================

	void vk_renderer::destroy_swapchain() {

		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		// destroy swapchain resources
		for (u64 i = 0; i < m_swapchain_image_views.size(); i++)
			vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);

	}

	
	void vk_renderer::resize_swapchain() {

		vkDeviceWaitIdle(m_device);

		destroy_swapchain();
		m_swapchain_extent.width = m_window->get_width();
		m_swapchain_extent.height = m_window->get_height();
		create_swapchain(m_swapchain_extent.width, m_swapchain_extent.height);

		m_resize_nedded = false;
	}


	void vk_renderer::draw_internal(VkCommandBuffer cmd) {

		if (m_current_background_effect < m_background_effects.size()) {

			render::compute_effect& effect = m_background_effects[m_current_background_effect];
	
			// bind the gradient drawing compute pipeline
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_gradient_pipeline_layout, 0, 1, &m_draw_image_descriptors, 0, nullptr);		// bind desc_set containing the draw_image for compute pipeline
			vkCmdPushConstants(cmd, m_gradient_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(render::compute_push_constants), &effect.data);
	
			// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
			vkCmdDispatch(cmd, static_cast<u32>(std::ceil(m_draw_extent.width / 16.0)), static_cast<u32>(std::ceil(m_draw_extent.height / 16.0)), 1);
		
		} else {

			
			render::compute_push_constants_dynamic_skybox skybox_data{};
			skybox_data.basic_sky_color = glm::vec4(0.3f, 0.15f, 0.25f, 1.f);
			skybox_data.sun_distance = 1000.f;
			skybox_data.sun_radius = .05f;

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_skybox_pipeline);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_skybox_pipeline_layout, 0, 1, &m_draw_image_descriptors, 0, nullptr);		// bind desc_set containing the draw_image for compute pipeline
			vkCmdPushConstants(cmd, m_skybox_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(skybox_data), &skybox_data);
			vkCmdDispatch(cmd, static_cast<u32>(std::ceil(m_draw_extent.width / 16.0)), static_cast<u32>(std::ceil(m_draw_extent.height / 16.0)), 1);
		}
	}

	
	void vk_renderer::draw_geometry(VkCommandBuffer cmd) {

		COLLECTING_PERFORMANCE_DATA(PFF::stopwatch loc_stopwatch(&m_renderer_metrik.draw_geometry_time[m_renderer_metrik.current_index]));

		// ========================================== create GPU global scene data ==========================================
		//set dynamic viewport and scissor
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<f32>(m_draw_extent.width);
		viewport.height = static_cast<f32>(m_draw_extent.height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_draw_extent.width;
		scissor.extent.height = m_draw_extent.height;
		vkCmdSetScissor(cmd, 0, 1, &scissor);

#ifndef TRY_TO_PROVIDE_SCNENE_DATA_TO_COMP
		m_scene_data.view = m_active_camera->get_view();
		m_scene_data.proj = glm::perspective(glm::radians(m_active_camera->get_perspective_fov_y()), (float)m_draw_extent.width / (float)m_draw_extent.height, 100000.f, 0.1f);
		m_active_camera->force_set_projection_matrix(m_scene_data.proj);
		m_scene_data.proj[1][1] *= -1;				// invert the Y direction on projection matrix
		m_scene_data.proj_view = m_scene_data.proj * m_scene_data.view;

		// allocate a new uniform buffer for the scene data
		vk_buffer gpuSceneDataBuffer = create_buffer(sizeof(render::GPU_scene_data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		get_current_frame().del_queue.push_func([this, gpuSceneDataBuffer]() { destroy_buffer(gpuSceneDataBuffer); });

		//write the buffer
		render::GPU_scene_data* scene_uniform_data = (render::GPU_scene_data*)gpuSceneDataBuffer.allocation->GetMappedData();
		*scene_uniform_data = m_scene_data;
		
		//create a descriptor set that binds that buffer and update it
		VkDescriptorSet globalDescriptor = get_current_frame().frame_descriptors.allocate(m_device, m_gpu_scene_data_descriptor_layout);
		descriptor_writer writer;
		writer.write_buffer(0, gpuSceneDataBuffer.buffer, sizeof(render::GPU_scene_data), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.update_set(m_device, globalDescriptor);
#endif // TRY_TO_PROVIDE_SCNENE_DATA_TO_COMP

		VkRenderingAttachmentInfo color_attachment = init::attachment_info(m_draw_image.get_image_view(), nullptr, VK_IMAGE_LAYOUT_GENERAL);					// begin a render pass connected to our draw image
		VkRenderingAttachmentInfo depth_attachment = init::depth_attachment_info(m_depth_image.get_image_view(), VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
		VkRenderingInfo render_info = init::rendering_info(m_draw_extent, &color_attachment, &depth_attachment);

		calc_frustum_planes(m_scene_data.proj_view);

		const auto& loc_map = application::get().get_world_layer()->get_map();										// TODO: implement world chunk system and iterat over chunks
		if (!loc_map->is_active()) {		// skip maps that are not-loaded/hidden/disabled

			return;
		}

		vkCmdBeginRendering(cmd, &render_info);

		
		// Grid drawing -------------------------------------------------
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_grid_pipeline);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_grid_pipeline_layout, 
			0,  // First set
			1,  // Descriptor count
			&m_global_descriptor,  // scene data descriptor
			0, nullptr );
		vkCmdDraw(cmd, 4, 1, 0, 0);  // 4 vertices


		ref<material_instance> last_material = nullptr;
		material_pipeline* last_pipeline = nullptr;


		// TODO: add high-level culling for map-chunks that don't need rendering
			// frustum culling
			// oclution culling (map-chunks may be to big & flat, so oclution may not make sense)


		if (script_system::is_ready()) {

			//get every entity with [transform] and [procedural_mesh]
			const auto procedural_view = loc_map->get_registry().view<procedural_mesh_component>();	// .group<transform_component>(entt::get<procedural_mesh_component>);
			for (const auto entity : procedural_view) {

				const auto& [procedural_mesh_comp] = procedural_view.get(entity);
				if (!procedural_mesh_comp.instance || !procedural_mesh_comp.shoudl_render)
					continue;

				const auto& transform_comp = loc_map->get_registry().get<transform_component>(entity);
				auto mesh_asset = procedural_mesh_comp.instance->get_mesh_asset();
				if (mesh_asset.surfaces.size() <= 0 || !is_bounds_in_frustum(mesh_asset.bounds_data, (glm::mat4&)transform_comp))
					continue;


				// TODO: add more culling for geometry that doesn't need to be drawns
					// oclution culling

				// only bind material and pipeline when needed
				ref<material_instance> loc_material = (mesh_asset.material != nullptr) ? mesh_asset.material : m_default_material;
				if (last_material != loc_material) {

					last_material = loc_material;
					material_pipeline* loc_pipeline = (loc_material->pipeline != nullptr) ? loc_material->pipeline : m_default_material->pipeline;
					if (last_pipeline != loc_pipeline) {

						last_pipeline = loc_pipeline;
						vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_pipeline->pipeline);
						vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_pipeline->layout, 0, 1, &m_global_descriptor, 0, nullptr);
						COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.pipline_binding_count++);
					}
					vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_material->pipeline->layout, (u32)1, (u32)1, &loc_material->material_set, (u32)0, nullptr);
					COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.material_binding_count++);
				}

				GPU_draw_push_constants push_constants;
				switch (procedural_mesh_comp.mobility_data) {
				case mobility::locked:		push_constants.world_matrix = (glm::mat4&)transform_comp; break;
				case mobility::movable:		push_constants.world_matrix = (glm::mat4&)transform_comp; break;		// TODO: meeds to check if object moved
				case mobility::dynamic:
				default:					push_constants.world_matrix = (glm::mat4&)transform_comp; break;		// TODO: check for relationship_comp => add parent transform
				}
				push_constants.vertex_buffer = mesh_asset.mesh_buffers.vertex_buffer_address;
				vkCmdPushConstants(cmd, loc_material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, (u32)0, sizeof(GPU_draw_push_constants), &push_constants);
				vkCmdBindIndexBuffer(cmd, mesh_asset.mesh_buffers.index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

// #define KRIPLE_RENDERER_TO_DEBUG_MESH_BOOLEAN_OPERATIONS
#ifdef KRIPLE_RENDERER_TO_DEBUG_MESH_BOOLEAN_OPERATIONS
				vkCmdDrawIndexed(cmd, mesh_asset.surfaces[2].count, 1, mesh_asset.surfaces[2].startIndex, 0, 0);		// POSIBLE OPIMIZATION - Collect all transforms of mesh_comp pointing to same mesh_asset and draw indexed

				COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.triangles += (u64)mesh_asset.surfaces[2].count / 3);
				COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.draw_calls++);
#else
				// Draw every surface in mesh_asset
				for (u64 x = 0; x < mesh_asset.surfaces.size(); x++) {

					vkCmdDrawIndexed(cmd, mesh_asset.surfaces[x].count, 1, mesh_asset.surfaces[x].startIndex, 0, 0);		// POSIBLE OPIMIZATION - Collect all transforms of mesh_comp pointing to same mesh_asset and draw indexed

					COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.triangles += (u64)mesh_asset.surfaces[x].count / 3);
					COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.draw_calls++);
				}
#endif

				COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.mesh_draw++);
			}

		}

		// get every entity with [transform] and [mesh]
		const auto group = loc_map->get_registry().group<transform_component>(entt::get<mesh_component>);
		for (const auto entity : group) {
				
			const auto& [transform_comp, mesh_comp] = group.get<transform_component, mesh_component>(entity);

			if (!mesh_comp.mesh_asset || mesh_comp.asset_path.empty() || !mesh_comp.shoudl_render)
				continue;

			if (!is_bounds_in_frustum(mesh_comp.mesh_asset->bounds_data, (glm::mat4&)transform_comp))
				continue;

			// TODO: add more culling for geometry that doesn't need to be drawns
				// oclution culling

			// only bind material and pipeline when needed
			ref<material_instance> loc_material = (mesh_comp.material != nullptr) ? mesh_comp.material : m_default_material;
			if (last_material != loc_material) {

				last_material = loc_material;
				material_pipeline* loc_pipeline = (loc_material->pipeline != nullptr) ? loc_material->pipeline : m_default_material->pipeline;
				if (last_pipeline != loc_pipeline) {

					last_pipeline = loc_pipeline;
					vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_pipeline->pipeline);
					vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_pipeline->layout, 0, 1, &m_global_descriptor, 0, nullptr);
					COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.pipline_binding_count++);
				}
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_material->pipeline->layout, (u32)1, (u32)1, &loc_material->material_set, (u32)0, nullptr);
				COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.material_binding_count++);
			}

			GPU_draw_push_constants push_constants;
			switch (mesh_comp.mobility_data) {
			case mobility::locked:		push_constants.world_matrix = (glm::mat4&)transform_comp; break;
			case mobility::movable:		push_constants.world_matrix = (glm::mat4&)transform_comp; break;		// TODO: meeds to check if object moved
			case mobility::dynamic:
			default:					push_constants.world_matrix = (glm::mat4&)transform_comp; break;		// TODO: check for relationship_comp => add parent transform
			}
			push_constants.vertex_buffer = mesh_comp.mesh_asset->mesh_buffers.vertex_buffer_address;
			vkCmdPushConstants(cmd, loc_material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, (u32)0, sizeof(GPU_draw_push_constants), &push_constants);
			vkCmdBindIndexBuffer(cmd, mesh_comp.mesh_asset->mesh_buffers.index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

			// Draw every surface in mesh_asset
			for (u64 x = 0; x < mesh_comp.mesh_asset->surfaces.size(); x++) {

				vkCmdDrawIndexed(cmd, mesh_comp.mesh_asset->surfaces[x].count, 1, mesh_comp.mesh_asset->surfaces[x].startIndex, 0, 0);		// POSIBLE OPIMIZATION - Collect all transforms of mesh_comp pointing to same mesh_asset and draw indexed

				COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.triangles += (u64)mesh_comp.mesh_asset->surfaces[x].count / 3);
				COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.draw_calls++);
			}

			COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.mesh_draw++);
		}

#ifdef PFF_RENDERER_DEBUG_CAPABILITY
		
		if (m_debug_lines.vertices.size() > 0) {
			
			// binding debug material for lines
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_debug_lines_material_inst.pipeline->pipeline);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_debug_lines_material_inst.pipeline->layout, 0, 1, &m_global_descriptor, 0, nullptr);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_debug_lines_material_inst.pipeline->layout, (u32)1, (u32)1, &m_debug_lines_material_inst.material_set, (u32)0, nullptr);
			
			GPU_draw_push_constants push_constants;
			push_constants.world_matrix = glm::mat4{1};
			push_constants.vertex_buffer = m_debug_lines.mesh_buffers.vertex_buffer_address;
			vkCmdPushConstants(cmd, m_debug_lines_material_inst.pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, (u32)0, sizeof(GPU_draw_push_constants), &push_constants);
			vkCmdBindIndexBuffer(cmd, m_debug_lines.mesh_buffers.index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

			for (u64 x = 0; x < m_debug_lines.surfaces.size(); x++) {

				vkCmdDrawIndexed(cmd, m_debug_lines.surfaces[x].count, 1, m_debug_lines.surfaces[x].startIndex, 0, 0);		// POSIBLE OPIMIZATION - Collect all transforms of mesh_comp pointing to same mesh_asset and draw indexed

				COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.triangles += (u64)m_debug_lines.surfaces[x].count / 3);
				COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.draw_calls++);
			}

			COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.pipline_binding_count++);
			COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.material_binding_count++);
			COLLECTING_PERFORMANCE_DATA(m_renderer_metrik.mesh_draw++);
		}

#endif // PFF_RENDERER_DEBUG_CAPABILITY

		vkCmdEndRendering(cmd);
	}


	// called once per frame
	void vk_renderer::calc_frustum_planes(const glm::mat4& pro_view) {

		//PFF_ISOLATED_PROFILER_SCOPED(700, "calc frustum planes ", PFF::duration_precision::microseconds);

		glm::mat4 m = glm::transpose(pro_view);
		m_view_frustum[0] = glm::vec4(m[3][0] + m[0][0], m[3][1] + m[0][1], m[3][2] + m[0][2], m[3][3] + m[0][3]);  // left plane
		m_view_frustum[1] = glm::vec4(m[3][0] - m[0][0], m[3][1] - m[0][1], m[3][2] - m[0][2], m[3][3] - m[0][3]);  // right plane
		m_view_frustum[2] = glm::vec4(m[3][0] + m[1][0], m[3][1] + m[1][1], m[3][2] + m[1][2], m[3][3] + m[1][3]);  // bottom plane
		m_view_frustum[3] = glm::vec4(m[3][0] - m[1][0], m[3][1] - m[1][1], m[3][2] - m[1][2], m[3][3] - m[1][3]);  // top plane
		m_view_frustum[4] = glm::vec4(m[3][0] + m[2][0], m[3][1] + m[2][1], m[3][2] + m[2][2], m[3][3] + m[2][3]);  // near plane
		m_view_frustum[5] = glm::vec4(m[3][0] - m[2][0], m[3][1] - m[2][1], m[3][2] - m[2][2], m[3][3] - m[2][3]);  // far plane

		// normalize planes
		for (auto& plane : m_view_frustum) {
			__m128 plane_sse = _mm_set_ps(plane.w, plane.z, plane.y, plane.x);
			__m128 length = _mm_sqrt_ps(_mm_dp_ps(plane_sse, plane_sse, 0x7F));
			plane_sse = _mm_div_ps(plane_sse, length);
			_mm_storeu_ps(&plane.x, plane_sse);
		}
	}

	// called for every object (map-chunk/mesh/...)
	bool vk_renderer::is_bounds_in_frustum(const PFF::geometry::bounds& bounds, const glm::mat4& transform) {

		//PFF_ISOLATED_PROFILER_SCOPED(1000000, "frustum bounds check", PFF::duration_precision::microseconds);

		glm::vec4 transformed_center = transform * glm::vec4(bounds.origin, 1.0f);
		__m128 center = _mm_set_ps(1.0f, transformed_center.z, transformed_center.y, transformed_center.x);

		f32 transformed_radius = glm::length(transform * glm::vec4(bounds.sphere_radius, bounds.sphere_radius, bounds.sphere_radius, 0.0f));
		__m128 radius = _mm_set1_ps(transformed_radius);
		__m128 neg_radius = _mm_sub_ps(_mm_setzero_ps(), radius);

		for (const auto& plane : m_view_frustum) {
			__m128 plane_vec = _mm_set_ps(plane.w, plane.z, plane.y, plane.x);
			__m128 dot_product = _mm_dp_ps(center, plane_vec, 0x7F);
			__m128 distance = _mm_add_ps(dot_product, _mm_set1_ps(plane.w));
			__m128 result = _mm_cmpgt_ps(distance, neg_radius);
			if (_mm_movemask_ps(result) != 0xF)								// at least one plane is outside, return false
				return false;
		}
		return true;
	}


	void vk_renderer::draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) {

		VkRenderingAttachmentInfo colorAttachment = init::attachment_info(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
		VkRenderingInfo renderInfo = init::rendering_info(m_swapchain_extent, &colorAttachment, nullptr);

		vkCmdBeginRendering(cmd, &renderInfo);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
		vkCmdEndRendering(cmd);
	}


	void vk_renderer::create_swapchain(u32 width, u32 height) {
				
		m_swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;
		VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
		vkb::Swapchain vkbSwapchain = vkb::SwapchainBuilder( m_chosenGPU, m_device, m_surface )
			//.use_default_format_selection()
			.set_desired_format(VkSurfaceFormatKHR{ m_swapchain_image_format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
			// VK_PRESENT_MODE_MAILBOX_KHR => fastest mode
			// VK_PRESENT_MODE_FIFO_KHR => vsync present mode
			.set_desired_present_mode(present_mode)
			.set_desired_extent(width, height)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.build().value();

		m_swapchain_extent = vkbSwapchain.extent;
		m_swapchain = vkbSwapchain.swapchain;
		m_swapchain_images = vkbSwapchain.get_images().value();
		m_swapchain_image_views = vkbSwapchain.get_image_views().value();
	}

	// =======================================================================================================================================
	// BUFFER
	// =======================================================================================================================================

	vk_buffer vk_renderer::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {

		VkBufferCreateInfo buffer_CI = {};
		buffer_CI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_CI.pNext = nullptr;
		buffer_CI.size = allocSize;
		buffer_CI.usage = usage;

		VmaAllocationCreateInfo vmaalloc_CI = {};
		vmaalloc_CI.usage = memoryUsage;
		vmaalloc_CI.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
		vk_buffer new_buffer;

		VK_CHECK_S(vmaCreateBuffer(m_allocator, &buffer_CI, &vmaalloc_CI, &new_buffer.buffer, &new_buffer.allocation, &new_buffer.info));
		return new_buffer;
	}


	void vk_renderer::destroy_buffer(const vk_buffer& buffer) { vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation); }

	
	render::GPU_mesh_buffers vk_renderer::upload_mesh(std::vector<u32> indices, std::vector<PFF::geometry::vertex> vertices) {

		const size_t vertexBufferSize = vertices.size() * sizeof(PFF::geometry::vertex);
		const size_t indexBufferSize = indices.size() * sizeof(u32);
		render::GPU_mesh_buffers new_mesh{};
		new_mesh.vertex_buffer = create_buffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		
		//find the adress of the vertex buffer
		VkBufferDeviceAddressInfo device_adress_I{};
		device_adress_I.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		device_adress_I.buffer = new_mesh.vertex_buffer.buffer;

		new_mesh.vertex_buffer_address = vkGetBufferDeviceAddress(m_device, &device_adress_I);
		new_mesh.index_buffer = create_buffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);		
		vk_buffer staging = create_buffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* data = staging.allocation->GetMappedData();
		
		memcpy(data, vertices.data(), vertexBufferSize);								// copy vertex buffer
		memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);		// copy index buffer

		immediate_submit([&](VkCommandBuffer cmd) {

			VkBufferCopy vertexCopy{ 0 };
			vertexCopy.dstOffset = 0;
			vertexCopy.srcOffset = 0;
			vertexCopy.size = vertexBufferSize;
			vkCmdCopyBuffer(cmd, staging.buffer, new_mesh.vertex_buffer.buffer, 1, &vertexCopy);

			VkBufferCopy indexCopy{ 0 };
			indexCopy.dstOffset = 0;
			indexCopy.srcOffset = vertexBufferSize;
			indexCopy.size = indexBufferSize;
			vkCmdCopyBuffer(cmd, staging.buffer, new_mesh.index_buffer.buffer, 1, &indexCopy);
		});

		destroy_buffer(staging);
		m_deletion_queue.push_func([this, new_mesh]() {

			destroy_buffer(new_mesh.vertex_buffer);
			destroy_buffer(new_mesh.index_buffer);
		});
		
		return new_mesh;
	}


	void vk_renderer::update_mesh(render::GPU_mesh_buffers& mesh, const std::vector<u32>& indices, const std::vector<PFF::geometry::vertex>& vertices) {
		
		const size_t vertexBufferSize = vertices.size() * sizeof(PFF::geometry::vertex);
		const size_t indexBufferSize = indices.size() * sizeof(u32);
		if (!mesh.vertex_buffer.buffer || !mesh.index_buffer.buffer ||
			mesh.vertex_buffer.info.size < vertexBufferSize || mesh.index_buffer.info.size < indexBufferSize) {

			//LOG(Warn, "Mesh not created yet or size changed, calling [upload_mesh()]");
			mesh = upload_mesh(indices, vertices);
			return;
		}

		vk_buffer staging = create_buffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);	// Create a staging buffer
		void* data = staging.allocation->GetMappedData();																						// Map the staging buffer

		// copy new data to the staging buffer
		memcpy(data, vertices.data(), vertexBufferSize);
		memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

		// submit a command to copy from staging buffer to the actual buffers
		immediate_submit([&](VkCommandBuffer cmd) {
			VkBufferCopy vertexCopy{};
			vertexCopy.dstOffset = 0;
			vertexCopy.srcOffset = 0;
			vertexCopy.size = vertexBufferSize;
			vkCmdCopyBuffer(cmd, staging.buffer, mesh.vertex_buffer.buffer, 1, &vertexCopy);

			VkBufferCopy indexCopy{};
			indexCopy.dstOffset = 0;
			indexCopy.srcOffset = vertexBufferSize;
			indexCopy.size = indexBufferSize;
			vkCmdCopyBuffer(cmd, staging.buffer, mesh.index_buffer.buffer, 1, &indexCopy);
		});
		destroy_buffer(staging);
	}
	

	void vk_renderer::update_mesh(PFF::geometry::procedural_mesh_asset& mesh, const std::vector<u32>& indices, const std::vector<PFF::geometry::vertex>& vertices) {

		const size_t vertexBufferSize = vertices.size() * sizeof(PFF::geometry::vertex);
		const size_t indexBufferSize = indices.size() * sizeof(u32);
		const size_t totalSize = vertexBufferSize + indexBufferSize;

		if (!mesh.mesh_buffers.vertex_buffer.buffer || !mesh.mesh_buffers.index_buffer.buffer
			|| mesh.mesh_buffers.vertex_buffer.info.size < vertexBufferSize || mesh.mesh_buffers.index_buffer.info.size < indexBufferSize) {

			//LOG(Warn, "Mesh not created yet or size changed, calling [upload_mesh()]");
			mesh.mesh_buffers = upload_mesh(indices, vertices);
			mesh.indices = indices;
			mesh.vertices = vertices;
			mesh.calc_bounds();
			return;
		}

		// Create or resize staging buffer if necessary
		if (mesh.staging_buffer_size < totalSize) {
			if (mesh.staging_buffer.buffer) {

				LOG(Info, "Destroying existing staging buffer");
				destroy_buffer(mesh.staging_buffer);
				mesh.staging_buffer = {};
				mesh.staging_data = nullptr;
				mesh.staging_buffer_size = 0;
			}
			
			LOG(Info, "Creating new staging buffer of size: [" << totalSize << "]");
			mesh.staging_buffer = create_buffer(totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
			VALIDATE(mesh.staging_buffer.buffer, return, "", "Failed to create staging buffer");

			mesh.staging_data = mesh.staging_buffer.allocation->GetMappedData();
			mesh.staging_buffer_size = totalSize;
		}

		// Copy new data to the staging buffer
		memcpy(mesh.staging_data, vertices.data(), vertexBufferSize);
		memcpy((char*)mesh.staging_data + vertexBufferSize, indices.data(), indexBufferSize);

		// Submit a command to copy from staging buffer to the actual buffers
		immediate_submit([&](VkCommandBuffer cmd) {
			VkBufferCopy vertexCopy{};
			vertexCopy.size = vertexBufferSize;
			vkCmdCopyBuffer(cmd, mesh.staging_buffer.buffer, mesh.mesh_buffers.vertex_buffer.buffer, 1, &vertexCopy);

			VkBufferCopy indexCopy{};
			indexCopy.srcOffset = vertexBufferSize;
			indexCopy.size = indexBufferSize;
			vkCmdCopyBuffer(cmd, mesh.staging_buffer.buffer, mesh.mesh_buffers.index_buffer.buffer, 1, &indexCopy);

			// Add memory barriers
			VkBufferMemoryBarrier barriers[2] = {};
			barriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			barriers[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barriers[0].dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
			barriers[0].buffer = mesh.mesh_buffers.vertex_buffer.buffer;
			barriers[0].size = VK_WHOLE_SIZE;

			barriers[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			barriers[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barriers[1].dstAccessMask = VK_ACCESS_INDEX_READ_BIT;
			barriers[1].buffer = mesh.mesh_buffers.index_buffer.buffer;
			barriers[1].size = VK_WHOLE_SIZE;

			vkCmdPipelineBarrier(cmd,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				0,
				0, nullptr,
				2, barriers,
				0, nullptr);
			});

		// Update the mesh data
		mesh.indices = indices;
		mesh.vertices = vertices;
		//mesh.calc_bounds();
	}


	void vk_renderer::cleanup_procedural_mesh(PFF::geometry::procedural_mesh_asset& mesh) {

		if (!mesh.staging_buffer.buffer || mesh.staging_buffer_size == 0 || !mesh.staging_data)
			return;

		destroy_buffer(mesh.staging_buffer);
		mesh.staging_buffer = {};
		mesh.staging_data = nullptr;
		mesh.staging_buffer_size = 0;
	}


	void vk_renderer::release_mesh(render::GPU_mesh_buffers& mesh) {

		if (mesh.vertex_buffer.buffer) {
		
			vmaDestroyBuffer(m_allocator, mesh.vertex_buffer.buffer, mesh.vertex_buffer.allocation);
			mesh.vertex_buffer = {};  // Reset to empty state
		}

		if (mesh.index_buffer.buffer) {
			
			vmaDestroyBuffer(m_allocator, mesh.index_buffer.buffer, mesh.index_buffer.allocation);
			mesh.index_buffer = {};  // Reset to empty state
		}
		mesh.vertex_buffer_address = 0;
		LOG(Trace, "Mesh released from GPU memory");
	}

#ifdef PFF_RENDERER_DEBUG_CAPABILITY

	void vk_renderer::add_debug_line(PFF::geometry::vertex start, PFF::geometry::vertex end) {

		m_debug_lines.vertices.push_back(start);
		m_debug_lines.vertices.push_back(end);

		size_t index = m_debug_lines.indices.size();
		m_debug_lines.indices.push_back((u32)index);
		m_debug_lines.indices.push_back((u32)index + 1);

		if (m_debug_lines.surfaces.size() == 0) {

			PFF::geometry::Geo_surface surface = { 0, (u32)m_debug_lines.indices.size(), PFF::geometry::bounds() };
			m_debug_lines.surfaces.emplace_back(surface);
		}

		m_debug_lines.surfaces[0].count = (u32)m_debug_lines.indices.size();
		update_mesh(m_debug_lines.mesh_buffers, m_debug_lines.indices, m_debug_lines.vertices);
	}


	void vk_renderer::clear_debug_line() {

		m_debug_lines.vertices.clear();
		m_debug_lines.indices.clear();
		m_debug_lines.surfaces[0].count = 0;
	}

#endif // PFF_RENDERER_DEBUG_CAPABILITY

}
