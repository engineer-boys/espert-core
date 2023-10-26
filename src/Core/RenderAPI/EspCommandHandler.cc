#include "EspCommandHandler.hh"

// #include "Platform/OpenGL/OpenGLCommandHandler.hh"
#include "Platform/Vulkan/VulkanCommandHandler.hh"

namespace esp
{
  bool EspCommandHandler::s_is_exist = false;

  std::unique_ptr<EspCommandHandler> EspCommandHandler::create_and_init()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto command_handler = VulkanCommandHandler::create();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    command_handler->init();
    return command_handler;
  }
} // namespace esp