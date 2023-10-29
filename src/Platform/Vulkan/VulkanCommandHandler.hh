#ifndef VULKAN_RENDER_API_VULKAN_COMMAND_MANAGER_HH
#define VULKAN_RENDER_API_VULKAN_COMMAND_MANAGER_HH

#include "Core/RenderAPI/EspCommandHandler.hh"

// libs
#include "volk.h"

namespace esp
{
  class VulkanCommandHandler : public EspCommandHandler
  {
   private:
    static VulkanCommandHandler* s_instance;

    VkCommandPool m_command_pool;

   public:
    static std::unique_ptr<VulkanCommandHandler> create();

    VulkanCommandHandler(const VulkanCommandHandler&)            = delete;
    VulkanCommandHandler& operator=(const VulkanCommandHandler&) = delete;

    // ------------------------ API ------------------------
    ~VulkanCommandHandler() override;

    void init() override;
    void terminate() override;

    static void draw(uint32_t vertex_count);
    static void draw(uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(uint32_t index_count);
    static void draw_indexed(uint32_t index_count, uint32_t instance_count);
    // -----------------------------------------------------

    static VkCommandBuffer create_command_buffer();
    static void free_command_buffer(VkCommandBuffer command_buffer);
    static VkCommandBuffer begin_single_time_commands();
    static void end_single_time_commands(VkCommandBuffer command_buffer);

   private:
    VulkanCommandHandler();

    void create_command_pool();
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_COMMAND_MANAGER_HH
