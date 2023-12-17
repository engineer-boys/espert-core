#ifndef VULKAN_RENDER_API_VULKAN_JOB_HH
#define VULKAN_RENDER_API_VULKAN_JOB_HH

// libs
#include "volk.h"

// Render API
#include "Core/RenderAPI/Work/EspJobs.hh"

namespace esp
{
  class VulkanJobs : public EspJobs
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    static VulkanJobs* s_instance;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanJobs();
    virtual ~VulkanJobs();

    VulkanJobs(const VulkanJobs& other)            = delete;
    VulkanJobs& operator=(const VulkanJobs& other) = delete;

    virtual void init() override;
    virtual void terminate() override;
    virtual void done_all_jobs() override;

    /* -------------------------- PUBLIC METHODS --------------------------- */
   public:
    static std::unique_ptr<VulkanJobs> create();

    static void draw(uint32_t vertex_count);
    static void draw(uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(uint32_t index_count);
    static void draw_indexed(uint32_t index_count, uint32_t instance_count);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_JOB_HH
