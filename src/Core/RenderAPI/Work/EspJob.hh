#ifndef RENDER_API_ESP_JOB_H
#define RENDER_API_ESP_JOB_H

#include "esppch.hh"

#include "Core/RenderAPI/RenderPlans/Block/Types/EspImageLayout.hh"
#include "Core/RenderAPI/RenderPlans/EspCommandBuffer.hh"
#include "Core/RenderAPI/Resources/EspImageCopy.hh"
#include "Core/RenderAPI/Resources/EspImageSubresourceRange.hh"
#include "Core/RenderAPI/Resources/EspTexture.hh"

namespace esp
{
  class EspJob
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspJob()          = default;
    virtual ~EspJob() = default;

    EspJob(const EspJob& other)            = delete;
    EspJob& operator=(const EspJob& other) = delete;

    virtual void init()          = 0;
    virtual void terminate()     = 0;
    virtual void done_all_jobs() = 0;

    static void draw(uint32_t vertex_count);
    static void draw(uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(uint32_t index_count, uint32_t instance_count = 1, uint32_t first_index = 0);

    static void draw(EspCommandBufferId* id, uint32_t vertex_count);
    static void draw(EspCommandBufferId* id, uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(EspCommandBufferId* id,
                             uint32_t index_count,
                             uint32_t instance_count = 1,
                             uint32_t first_index    = 0);

    static void copy_image(EspCommandBufferId* id,
                           std::shared_ptr<EspTexture> src_texture,
                           EspImageLayout src_layout,
                           std::shared_ptr<EspTexture> dst_texture,
                           EspImageLayout dst_layout,
                           EspImageCopy region);

    static void transform_image_layout(EspCommandBufferId* id,
                                       std::shared_ptr<EspTexture> texture,
                                       EspImageLayout old_layout,
                                       EspImageLayout new_layout,
                                       EspImageSubresourceRange image_subresource_range);

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspJob> build();
  };
} // namespace esp

#endif // RENDER_API_ESP_JOB_H
