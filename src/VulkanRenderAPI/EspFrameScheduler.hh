#ifndef ESP_RENDERER_H_
#define ESP_RENDERER_H_

#include "Core/EspWindow.hh"
#include "EspDevice.hh"
#include "EspSwapChain.hh"

//std
#include <cassert>
#include <memory>
#include <vector>

namespace esp
{
	class EspFrameScheduler
	{
	 private:
		EspWindow& m_window;
		EspDevice& m_device;
		std::unique_ptr<EspSwapChain> m_swap_chain;
		std::vector<VkCommandBuffer> m_command_buffers;

		uint32_t m_current_image_index;
		int m_current_frame_index{ 0};
		bool m_frame_started{ false};

	 public:
		EspFrameScheduler(EspWindow& window, EspDevice& device);
		~EspFrameScheduler();

		EspFrameScheduler(const EspFrameScheduler&) = delete;
		EspFrameScheduler& operator=(const EspFrameScheduler&) = delete;

		inline bool is_frame_in_progress() const { return m_frame_started; };

		inline VkCommandBuffer get_current_command_buffer() const
		{
			assert(m_frame_started && "Cannot get command buffer if frame hasn't started");
			return m_command_buffers[m_current_frame_index];
		}

		inline int get_current_frame_index()
		{
			assert(m_frame_started && "Cannot get frame index if frame hasn't started");
			return m_current_frame_index;
		}

		VkCommandBuffer begin_frame();
		void end_frame();

		inline VkRenderPass get_swap_chain_render_pass() const { return m_swap_chain->get_render_pass(); };
		inline float get_aspect_ratio() const { return m_swap_chain->get_swap_chain_extent_aspect_ratio(); };
		void begin_swap_chain_render_pass(VkCommandBuffer command_buffer);
		void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

	 private:
		void create_command_buffers();
		void free_command_buffers();
		void recreate_swap_chain();

		friend class EspRenderer;
	};
}

#endif //ESP_RENDERER_H_
