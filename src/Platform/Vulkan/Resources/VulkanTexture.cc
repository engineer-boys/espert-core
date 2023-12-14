#include "VulkanTexture.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

namespace esp
{
  VulkanTexture::VulkanTexture(const std::string& name,
                               const uint8_t* pixels,
                               uint8_t channel_count,
                               uint32_t width,
                               uint32_t height) :
      EspTexture(name, pixels, channel_count, width, height)
  {
  }

  std::shared_ptr<VulkanTexture> VulkanTexture::create(const std::string name,
                                                       std::unique_ptr<ImageResource> image,
                                                       bool mipmapping)
  {
    auto vulkan_texture =
        std::shared_ptr<VulkanTexture>(new VulkanTexture(name,
                                                         static_cast<const uint8_t*>(image->get_data()),
                                                         image->get_channel_count(),
                                                         image->get_width(),
                                                         image->get_height()));

    VulkanResourceManager::create_texture_image(vulkan_texture->get_width(),
                                                vulkan_texture->get_height(),
                                                image->release(),
                                                vulkan_texture->get_mip_levels(),
                                                vulkan_texture->m_texture_image,
                                                vulkan_texture->m_texture_image_memory);

    vulkan_texture->m_texture_image_view = VulkanResourceManager::create_image_view(vulkan_texture->m_texture_image,
                                                                                    VK_FORMAT_R8G8B8A8_SRGB,
                                                                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                                                                    vulkan_texture->m_mip_levels);

    vulkan_texture->m_sampler =
        !mipmapping ? VulkanSampler::get_default_sampler() : VulkanSampler::create(vulkan_texture->m_mip_levels);

    return vulkan_texture;
  }

  VulkanTexture::~VulkanTexture()
  {
    vkDestroyImageView(VulkanDevice::get_logical_device(), m_texture_image_view, nullptr);
    vkDestroyImage(VulkanDevice::get_logical_device(), m_texture_image, nullptr);
    vkFreeMemory(VulkanDevice::get_logical_device(), m_texture_image_memory, nullptr);
  }
} // namespace esp