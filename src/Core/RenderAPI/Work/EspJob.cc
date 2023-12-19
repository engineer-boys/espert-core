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
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto command_handler = VulkanJob::create();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    return command_handler;
  }

  void EspJob::draw(uint32_t vertex_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    VulkanJob::draw(vertex_count);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::draw(uint32_t vertex_count, uint32_t instance_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    VulkanJob::draw(vertex_count, instance_count);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }

  void EspJob::draw_indexed(uint32_t index_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    VulkanJob::draw_indexed(index_count);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }
  void EspJob::draw_indexed(uint32_t index_count, uint32_t instance_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    VulkanJob::draw_indexed(index_count, instance_count);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp
