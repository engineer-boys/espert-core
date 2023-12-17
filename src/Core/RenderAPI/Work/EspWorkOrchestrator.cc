#include "EspWorkOrchestrator.hh"

// #include "Platform/OpenGL/OpenGLFrameScheduler.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  std::unique_ptr<EspWorkOrchestrator> EspWorkOrchestrator::build()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto work_orchestrat = VulkanWorkOrchestrator::create();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
    return work_orchestrat;
  }

  std::pair<uint32_t, uint32_t> EspWorkOrchestrator::get_swap_chain_extent()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    return VulkanWorkOrchestrator::get_swap_chain_extent();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }

  float EspWorkOrchestrator::get_swap_chain_extent_aspect_ratio()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    return VulkanWorkOrchestrator::get_swap_chain_extent_aspect_ratio();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp