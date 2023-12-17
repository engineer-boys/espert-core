#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Platform/Vulkan/Resources/VulkanTexture.hh"

namespace esp
{
  std::shared_ptr<EspTexture> EspTexture::create(const std::string& name,
                                                 std::unique_ptr<ImageResource> image,
                                                 EspTextureType type,
                                                 bool mipmapping)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto texture = VulkanTexture::create(name, std::move(image), type, mipmapping);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    return texture;
  }

  EspTexture::EspTexture(const std::string& name,
                         const uint8_t* pixels,
                         uint8_t channel_count,
                         uint32_t width,
                         uint32_t height,
                         EspTextureType type) :
      m_name(name),
      m_channel_count(channel_count), m_width(width), m_height(height), m_has_transparency(false), m_type(type)
  {
    calculate_mip_levels();
    check_for_transparency(pixels);
  }

  EspTexture::EspTexture(uint32_t width, uint32_t height) :
      m_name(""), m_channel_count(0), m_width(width), m_height(height), m_has_transparency(false)
  {
    m_mip_levels = 1;
  }

  void EspTexture::calculate_mip_levels() { m_mip_levels = std::floor(std::log2(std::max(m_width, m_height))) + 1; }

  void EspTexture::check_for_transparency(const uint8_t* pixels)
  {
    if (m_channel_count != 2 && m_channel_count != 4) { return; }
    for (int i = 0; i < get_size(); i += m_channel_count)
    {
      if (pixels[i + 3] < 255)
      {
        m_has_transparency = true;
        break;
      }
    }
  }
} // namespace esp
