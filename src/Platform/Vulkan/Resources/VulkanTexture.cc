#include "VulkanTexture.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

namespace esp
{
  std::unique_ptr<VulkanTexture> VulkanTexture::create(const std::shared_ptr<Texture> texture, bool mipmapping)
  {
    auto vulkan_texture          = std::unique_ptr<VulkanTexture>(new VulkanTexture());
    vulkan_texture->m_width      = texture->get_width();
    vulkan_texture->m_height     = texture->get_height();
    vulkan_texture->m_mip_levels = texture->get_mip_levels();

    VulkanResourceManager::create_texture_image(texture,
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