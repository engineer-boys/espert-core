#ifndef VULKAN_RENDER_API_VULKAN_WORK_ORCHESTRATOR_HH
#define VULKAN_RENDER_API_VULKAN_WORK_ORCHESTRATOR_HH

// libs
#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Work/EspWorkOrchestrator.hh"
#include "VulkanJob.hh"
#include "VulkanSwapChain.hh"

namespace esp
{
  class VulkanWorkOrchestrator : public EspWorkOrchestrator
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    static VulkanWorkOrchestrator* s_instance;

    VkCommandPool m_command_pool;
    std::vector<VkCommandBuffer> m_command_buffers;

    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;

    std::unique_ptr<VulkanSwapChain> m_swap_chain;

    PFN_vkCmdBeginRenderingKHR m_vkCmdbeginRenderingKHR;
    PFN_vkCmdEndRenderingKHR m_vkCmdEndRenderingKHR;

    /* -------------------------- METHODS ---------------------------------- */
   private:
    void create_command_pool();
    void create_command_buffers();
    void create_sync_objects();
    void load_extension_functions();

   public:
    VulkanWorkOrchestrator();
    virtual ~VulkanWorkOrchestrator();

    VulkanWorkOrchestrator(const VulkanWorkOrchestrator& other)            = delete;
    VulkanWorkOrchestrator& operator=(const VulkanWorkOrchestrator& other) = delete;

    virtual void init(EspPresentationMode presentation_mode) override;
    virtual void terminate() override;

    virtual void begin_frame() override;
    virtual void end_frame() override;

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<VulkanWorkOrchestrator> create(EspPresentationMode presentation_mode);

    static void split_frame();

    inline static uint32_t get_number_of_command_buffers() { return s_instance->m_command_buffers.size(); }
    static VkCommandBuffer begin_single_time_commands();
    static void end_single_time_commands(VkCommandBuffer command_buffer);
    inline static std::pair<uint32_t, uint32_t> get_swap_chain_extent()
    {
      return { s_instance->m_swap_chain->m_swap_chain_extent.width,
               s_instance->m_swap_chain->m_swap_chain_extent.height };
    }
    inline static float get_swap_chain_extent_aspect_ratio()
    {
      return static_cast<float>(s_instance->m_swap_chain->m_swap_chain_extent.width) /
          static_cast<float>(s_instance->m_swap_chain->m_swap_chain_extent.height);
    }
    inline static VkCommandBuffer get_current_command_buffer()
    {
      return s_instance->m_command_buffers[s_instance->m_swap_chain->m_current_frame];
    }
    inline static VkCommandBuffer get_command_buffer(uint32_t command_buffer_idx)
    {
      return s_instance->m_command_buffers[command_buffer_idx];
    }

    inline static void insert_image_memory_barrier(VkCommandBuffer cmdbuffer,
                                                   VkImage image,
                                                   VkAccessFlags src_access_mask,
                                                   VkAccessFlags dst_access_mask,
                                                   VkImageLayout old_image_layout,
                                                   VkImageLayout new_image_layout,
                                                   VkPipelineStageFlags src_stage_mask,
                                                   VkPipelineStageFlags dst_stage_mask,
                                                   VkImageSubresourceRange subresource_range)
    {
      VkImageMemoryBarrier image_memory_barrier_info = {};
      image_memory_barrier_info.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      image_memory_barrier_info.srcAccessMask        = src_access_mask;
      image_memory_barrier_info.dstAccessMask        = dst_access_mask;
      image_memory_barrier_info.oldLayout            = old_image_layout;
      image_memory_barrier_info.newLayout            = new_image_layout;
      image_memory_barrier_info.image                = image;
      image_memory_barrier_info.subresourceRange     = subresource_range;

      vkCmdPipelineBarrier(cmdbuffer,
                           src_stage_mask,
                           dst_stage_mask,
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           1,
                           &image_memory_barrier_info);
    }

    inline static void insert_image_memory_barrier_to_current_cmdbuffer(VkImage image,
                                                                        VkAccessFlags src_access_mask,
                                                                        VkAccessFlags dst_access_mask,
                                                                        VkImageLayout old_image_layout,
                                                                        VkImageLayout new_image_layout,
                                                                        VkPipelineStageFlags src_stage_mask,
                                                                        VkPipelineStageFlags dst_stage_mask,
                                                                        VkImageSubresourceRange subresource_range)
    {
      insert_image_memory_barrier(get_current_command_buffer(),
                                  image,
                                  src_access_mask,
                                  dst_access_mask,
                                  old_image_layout,
                                  new_image_layout,
                                  src_stage_mask,
                                  dst_stage_mask,
                                  subresource_range);
    }

    inline static void begin_rendering(const VkRenderingInfo* info)
    {
      s_instance->m_vkCmdbeginRenderingKHR(s_instance->m_command_buffers[s_instance->m_swap_chain->m_current_frame],
                                           info);
    }

    inline static void end_rendering()
    {
      s_instance->m_vkCmdEndRenderingKHR(s_instance->m_command_buffers[s_instance->m_swap_chain->m_current_frame]);
    }

    inline static void begin_rendering(VkCommandBuffer command_buffer, const VkRenderingInfo* info)
    {
      s_instance->m_vkCmdbeginRenderingKHR(command_buffer, info);
    }

    inline static void end_rendering(VkCommandBuffer command_buffer)
    {
      s_instance->m_vkCmdEndRenderingKHR(command_buffer);
    }
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_WORK_ORCHESTRATOR_HH
