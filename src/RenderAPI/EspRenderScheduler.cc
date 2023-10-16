#include "EspRenderScheduler.hh"

// std
#include <stdexcept>
#include <array>

namespace esp
{
	EspRenderScheduler::EspRenderScheduler(EspWindow& window, EspDevice& device)
	: m_window{window}, m_device{device}
	{
		recreate_swap_chain();
		create_command_buffers();
	}

	EspRenderScheduler::~EspRenderScheduler()
	{
		free_command_buffers();
	}

	VkCommandBuffer EspRenderScheduler::begin_frame()
	{
		assert(!m_frame_started && "Cannot call begin_frame while frame already in progress");

		auto result = m_swap_chain->acquire_next_image(&m_current_image_index);

		//window has changed in such a way that is no longer compatible with the swap chain (example: resizing)
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreate_swap_chain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			ESP_CORE_ERROR("Failed to acquire swap chain image");
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		m_frame_started = true;

		auto command_buffer = get_current_command_buffer();
		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
		{
			ESP_CORE_ERROR("Failed to begin recording command buffer");
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		return command_buffer;
	}

	void EspRenderScheduler::end_frame()
	{
		assert(m_frame_started && "Cannot call end_frame while frame is not in progress");

		auto command_buffer = get_current_command_buffer();
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			ESP_CORE_ERROR("Failed to record command buffer");
			throw std::runtime_error("Failed to record command buffer");
		}

		auto result = m_swap_chain->submit_command_buffers(&command_buffer, &m_current_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			recreate_swap_chain();
		}
		else if (result != VK_SUCCESS)
		{
			ESP_CORE_ERROR("Failed to present swap chain image");
			throw std::runtime_error("Failed to present swap chain image");
		}

		m_frame_started = false;
		m_current_frame_index = (m_current_frame_index + 1) % EspSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void EspRenderScheduler::begin_swap_chain_render_pass(VkCommandBuffer command_buffer)
	{
		assert(m_frame_started && "Cannot call begin_swap_chain_render_pass while frame is not in progress");
		assert(command_buffer == get_current_command_buffer()
			&& "Cannot begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_swap_chain->get_render_pass();
		render_pass_info.framebuffer = m_swap_chain->get_frame_buffer(m_current_image_index);
		//renderArea - area where shader loads and stores will take place
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = m_swap_chain->get_swap_chain_extent();

		//clear_values - values the frame buffer will use to clear itself
		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.1f, 0.1f, 0.1f, 1.0f }; //color attachment
		clear_values[1].depthStencil = { 1.0f, 0 };                                  //depth attachment

		render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_info.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swap_chain->get_swap_chain_extent().width);
		viewport.height = static_cast<float>(m_swap_chain->get_swap_chain_extent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{ 0, 0 }, m_swap_chain->get_swap_chain_extent() };
		vkCmdSetViewport(command_buffer, 0, 1, &viewport);
		vkCmdSetScissor(command_buffer, 0, 1, &scissor);
	}

	void EspRenderScheduler::end_swap_chain_render_pass(VkCommandBuffer command_buffer)
	{
		assert(m_frame_started && "Cannot call end_swap_chain_render_pass while frame is in progress");
		assert(command_buffer == get_current_command_buffer()
			&& "Cannot end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(command_buffer);
	}

	void EspRenderScheduler::create_command_buffers()
	{
		m_command_buffers.resize(EspSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.level =
			VK_COMMAND_BUFFER_LEVEL_PRIMARY; //primary - can be submitted to device queue but can't be called by other command buffers
		allocate_info.commandPool = m_device.get_command_pool();
		allocate_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

		if (vkAllocateCommandBuffers(m_device.get_device(), &allocate_info, m_command_buffers.data()) != VK_SUCCESS)
		{
			ESP_CORE_ERROR("Failed to allocate command buffers");
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void EspRenderScheduler::free_command_buffers()
	{
		vkFreeCommandBuffers(
			m_device.get_device(),
			m_device.get_command_pool(),
			static_cast<uint32_t>(m_command_buffers.size()),
			m_command_buffers.data());
		m_command_buffers.clear();
	}

	void EspRenderScheduler::recreate_swap_chain()
	{
		//as long as 1 dimension is 0 (example: minimizing window) we wait
		auto extent = m_window.get_extent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_window.get_extent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_device.get_device()); //wait until the current swap chain is not being used

		if (m_swap_chain == nullptr)
		{
			m_swap_chain = std::make_unique<EspSwapChain>(m_device, extent);
		}
		else
		{
			std::shared_ptr<EspSwapChain> old_swap_chain = std::move(m_swap_chain);
			m_swap_chain = std::make_unique<EspSwapChain>(m_device, extent, old_swap_chain);

			if (!old_swap_chain->compare_swap_chain_formats(*m_swap_chain))
			{
				ESP_CORE_ERROR("Swap chain image/depth format has changed");
				throw std::runtime_error("Swap chain image/depth format has changed");
			}
		}
	}
}