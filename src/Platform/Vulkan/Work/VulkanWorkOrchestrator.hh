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

    PFN_vkCmdBeginRenderingKHR vkCmdbeginRenderingKHR;
    PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;

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

    virtual void init() override;
    virtual void terminate() override;

    virtual void begin_frame() override;
    virtual void end_frame() override;

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<VulkanWorkOrchestrator> create();

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

    inline static void insert_image_memory_barrier(VkCommandBuffer cmdbuffer,
                                                   VkImage image,
                                                   VkAccessFlags srcAccessMask,
                                                   VkAccessFlags dstAccessMask,
                                                   VkImageLayout oldImageLayout,
                                                   VkImageLayout newImageLayout,
                                                   VkPipelineStageFlags srcStageMask,
                                                   VkPipelineStageFlags dstStageMask,
                                                   VkImageSubresourceRange subresourceRange)
    {
      VkImageMemoryBarrier image_memory_barrier_info = {};
      image_memory_barrier_info.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      image_memory_barrier_info.srcAccessMask        = srcAccessMask;
      image_memory_barrier_info.dstAccessMask        = dstAccessMask;
      image_memory_barrier_info.oldLayout            = oldImageLayout;
      image_memory_barrier_info.newLayout            = newImageLayout;
      image_memory_barrier_info.image                = image;
      image_memory_barrier_info.subresourceRange     = subresourceRange;

      vkCmdPipelineBarrier(cmdbuffer,
                           srcStageMask,
                           dstStageMask,
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           1,
                           &image_memory_barrier_info);
    }

    inline static void insert_image_memory_barrier_to_current_cmdbuffer(VkImage image,
                                                                        VkAccessFlags srcAccessMask,
                                                                        VkAccessFlags dstAccessMask,
                                                                        VkImageLayout oldImageLayout,
                                                                        VkImageLayout newImageLayout,
                                                                        VkPipelineStageFlags srcStageMask,
                                                                        VkPipelineStageFlags dstStageMask,
                                                                        VkImageSubresourceRange subresourceRange)
    {
      insert_image_memory_barrier(get_current_command_buffer(),
                                  image,
                                  srcAccessMask,
                                  dstAccessMask,
                                  oldImageLayout,
                                  newImageLayout,
                                  srcStageMask,
                                  dstStageMask,
                                  subresourceRange);
    }

    inline static void begin_rendering(const VkRenderingInfo* info)
    {
      s_instance->vkCmdbeginRenderingKHR(s_instance->m_command_buffers[s_instance->m_swap_chain->m_current_frame],
                                         info);
    }

    inline static void end_rendering()
    {
      s_instance->vkCmdEndRenderingKHR(s_instance->m_command_buffers[s_instance->m_swap_chain->m_current_frame]);
    }
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_WORK_ORCHESTRATOR_HH
