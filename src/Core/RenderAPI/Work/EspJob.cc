#include "EspJob.hh"

// #include "Platform/OpenGL/OpenGLCommandHandler.hh"
#include "Platform/Vulkan/Work/VulkanJob.hh"

namespace esp
{
  std::unique_ptr<EspJob> EspJob::build()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    auto command_handler = VulkanJob::create();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/

    return command_handler;
  }

  void EspJob::draw(uint32_t vertex_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanJob::draw(vertex_count);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::draw(uint32_t vertex_count, uint32_t instance_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanJob::draw(vertex_count, instance_count);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanJob::draw_indexed(index_count, instance_count, first_index);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::draw(EspCommandBufferId* id, uint32_t vertex_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanJob::draw(id, vertex_count);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::draw(EspCommandBufferId* id, uint32_t vertex_count, uint32_t instance_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanJob::draw(id, vertex_count, instance_count);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::draw_indexed(EspCommandBufferId* id, uint32_t index_count, uint32_t instance_count, uint32_t first_index)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanJob::draw_indexed(id, index_count, instance_count, first_index);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::copy_image(EspCommandBufferId* id,
                          std::shared_ptr<EspTexture> src_texture,
                          EspImageLayout src_layout,
                          std::shared_ptr<EspTexture> dst_texture,
                          EspImageLayout dst_layout,
                          EspImageCopy region)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanJob::copy_image(id, src_texture, src_layout, dst_texture, dst_layout, region);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::transform_image_layout(EspCommandBufferId* id,
                                      std::shared_ptr<EspTexture> texture,
                                      EspImageLayout old_layout,
                                      EspImageLayout new_layout,
                                      EspImageSubresourceRange image_subresource_range)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanJob::transform_image_layout(id, texture, old_layout, new_layout, image_subresource_range);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp
