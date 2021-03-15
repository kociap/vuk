#pragma once

#include "vuk/Context.hpp"

namespace vuk {
	struct LinearResourceAllocator;

	struct LinearResourceAllocator {
		vuk::Context* ctx;
		uint32_t queue_family_index;
		VkCommandPool cpool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> command_buffers;
		vuk::Buffer buffer;
		std::vector<vuk::Image> images;
		std::vector<vuk::ImageView> image_views;
		VkFence fence = VK_NULL_HANDLE;
		VkSemaphore sema = VK_NULL_HANDLE;
		LinearResourceAllocator* next = nullptr;

		LinearResourceAllocator clone();

		Context& get_context() {
			return *ctx;
		}

		VkCommandBuffer acquire_command_buffer(VkCommandBufferLevel level) {
			VkCommandBuffer cbuf;
			VkCommandBufferAllocateInfo cbai{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			cbai.commandPool = cpool;
			cbai.commandBufferCount = 1;
			cbai.level = level;

			assert(vkAllocateCommandBuffers(ctx->device, &cbai, &cbuf) == VK_SUCCESS);
			command_buffers.push_back(cbuf);
			return cbuf;
		}

		VkFence acquire_fence();
		VkSemaphore acquire_semaphore();
		VkSemaphore acquire_timeline_semaphore();
		VkFramebuffer acquire_framebuffer(const struct FramebufferCreateInfo&);
		VkRenderPass acquire_renderpass(const struct RenderPassCreateInfo&);
		RGImage acquire_rendertarget(const struct RGCI&);
		Sampler acquire_sampler(const SamplerCreateInfo&);
		DescriptorSet acquire_descriptorset(const SetBinding&);
		PipelineInfo acquire_pipeline(const PipelineInstanceCreateInfo&);
		TimestampQuery register_timestamp_query(Query);

		Buffer allocate_scratch_buffer(MemoryUsage mem_usage, vuk::BufferUsageFlags buffer_usage, size_t size, size_t alignment);
	};
}