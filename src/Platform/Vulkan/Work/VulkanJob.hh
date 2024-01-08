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

    static void draw(EspCommandBufferId* id, uint32_t vertex_count);
    static void draw(EspCommandBufferId* id, uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(EspCommandBufferId* id, uint32_t index_count);
    static void draw_indexed(EspCommandBufferId* id, uint32_t index_count, uint32_t instance_count);

    static void copy_image(EspCommandBufferId* id,
                           std::shared_ptr<EspTexture> src_texture,
                           EspImageLayout src_layout,
                           std::shared_ptr<EspTexture> dst_texture,
                           EspImageLayout dst_layout,EspImageCopy region);

    static void transform_image_layout(EspCommandBufferId* id,
                                       std::shared_ptr<EspTexture> texture,
                                       EspImageLayout old_layout,
                                       EspImageLayout new_layout,
                                       EspImageSubresourceRange image_subresource_range);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_JOB_HH
