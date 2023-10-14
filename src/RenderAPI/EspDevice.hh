#ifndef RENDERER_ESP_DEVICE_H_
#define RENDERER_ESP_DEVICE_H_

#include "CoreSystems/EspWindow.hh"

// std
#include <string>
#include <vector>

namespace esp
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR m_capabilities;
		std::vector<VkSurfaceFormatKHR> m_formats;
		std::vector<VkPresentModeKHR> m_present_modes;
	};

	struct QueueFamilyIndices
	{
		uint32_t m_graphics_family;
		uint32_t m_present_family;
		bool m_graphics_family_has_value = false;
		bool m_present_family_has_value = false;
		bool isComplete() const
		{
			return m_graphics_family_has_value && m_present_family_has_value;
		}
	};

	class EspDevice
	{
	 private:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debug_messenger;
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		EspWindow& m_window;
		VkCommandPool m_command_pool;

		VkDevice m_device;
		VkSurfaceKHR m_surface;
		VkQueue m_graphics_queue;
		VkQueue m_present_queue;

		VkPhysicalDeviceProperties m_properties;

		const std::vector<const char*> m_validation_layers =
			{
				"VK_LAYER_KHRONOS_validation"
			};
		const std::vector<const char*> m_instance_extensions =
			{
				VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
				"VK_KHR_get_physical_device_properties2"
			};
		const std::vector<const char*> m_device_extensions =
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				"VK_KHR_portability_subset"
			};

	 public:
#ifdef NDEBUG
		const bool m_enable_validation_layers = false;
#else
		const bool m_enable_validation_layers = true;
#endif

		EspDevice(EspWindow& window);
		~EspDevice();

		// Not copyable or movable
		EspDevice(const EspDevice&) = delete;
		EspDevice& operator=(const EspDevice&) = delete;
		EspDevice(EspDevice&&) = delete;
		EspDevice& operator=(EspDevice&&) = delete;

		inline VkCommandPool get_command_pool() { return m_command_pool; }
		inline VkDevice get_device() { return m_device; }
		inline VkSurfaceKHR get_surface() { return m_surface; }
		inline VkQueue get_graphics_queue() { return m_graphics_queue; }
		inline VkQueue get_present_queue() { return m_present_queue; }
		inline SwapChainSupportDetails get_swap_chain_support() { return query_swap_chain_support(m_physical_device); }

		uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
		inline QueueFamilyIndices find_physical_queue_families() { return find_queue_families(m_physical_device); }
		VkFormat find_supported_format(
			const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		inline void complete_queues() { vkDeviceWaitIdle(m_device); }

		// Buffer Helper Functions
		void create_buffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& buffer_memory);
		VkCommandBuffer begin_single_time_commands();
		void end_single_time_commands(VkCommandBuffer command_buffer);
		void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
		void copy_buffer_to_image(
			VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count);

		void create_image_with_info(
			const VkImageCreateInfo& image_info,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& image_memory);

	 private:
		void create_instance();
		void setup_debug_messenger();
		void create_surface();
		void pick_physical_device();
		void create_logical_device();
		void create_command_pool();

		// helper functions
		bool is_device_suitable(VkPhysicalDevice device);
		std::vector<const char*> get_required_extensions();
		bool check_validation_layer_support();
		QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
		void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
		void has_glfw_required_instance_extensions();
		bool check_device_extension_support(VkPhysicalDevice device);
		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
	};

}

#endif //RENDERER_ESP_DEVICE_H_
