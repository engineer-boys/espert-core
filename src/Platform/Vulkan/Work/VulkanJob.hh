#ifndef VULKAN_RENDER_API_VULKAN_JOB_HH
#define VULKAN_RENDER_API_VULKAN_JOB_HH

// libs
#include "volk.h"

// Render API
#include "Core/RenderAPI/Work/EspJob.hh"

namespace esp
{
  class VulkanJob : public EspJob
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    static VulkanJob* s_instance;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanJob();
    virtual ~VulkanJob();

    VulkanJob(const VulkanJob& other)            = delete;
    VulkanJob& operator=(const VulkanJob& other) = delete;

    virtual void init() override;
    virtual void terminate() override;
    virtual void done_all_jobs() override;

    /* -------------------------- PUBLIC METHODS --------------------------- */
   public:
    static std::unique_ptr<VulkanJob> create();

    static void draw(uint32_t vertex_count);
    static void draw(uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(uint32_t index_count);
    static void draw_indexed(uint32_t index_count, uint32_t instance_count);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_JOB_HH
