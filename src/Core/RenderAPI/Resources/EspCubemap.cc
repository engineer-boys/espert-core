#include "Core/RenderAPI/Resources/EspCubemap.hh"
#include "Platform/Vulkan/Resources/VulkanCubemap.hh"

namespace esp
{
  std::shared_ptr<EspCubemap> EspCubemap::create(const std::string& name,
                                                 std::unique_ptr<CubemapResource> cubemap_resource)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto cubemap = VulkanCubemap::create(name, std::move(cubemap_resource));
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    return cubemap;
  }

  EspCubemap::EspCubemap(const std::string& name, uint8_t channel_count, uint32_t width, uint32_t height) :
      m_name(name), m_channel_count(channel_count), m_width(width), m_height(height), m_has_transparency(false)
  {
  }

} // namespace esp
