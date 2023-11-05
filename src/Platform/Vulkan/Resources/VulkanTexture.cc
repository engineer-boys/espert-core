#include "VulkanTexture.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

namespace esp
{
  std::unique_ptr<VulkanTexture> VulkanTexture::create(const std::string& path)
  {
    auto texture = std::unique_ptr<VulkanTexture>(new VulkanTexture());

    VulkanResourceManager::create_texture_image(path, texture->m_texture_image, texture->m_texture_image_memory);

    texture->m_texture_image_view = VulkanResourceManager::create_image_view(texture->m_texture_image,
                                                                             VK_FORMAT_R8G8B8A8_SRGB,
                                                                             VK_IMAGE_ASPECT_COLOR_BIT);

    return texture;
  }

  VulkanTexture::~VulkanTexture()
  {
    vkDestroyImageView(VulkanDevice::get_logical_device(), m_texture_image_view, nullptr);
    vkDestroyImage(VulkanDevice::get_logical_device(), m_texture_image, nullptr);
    vkFreeMemory(VulkanDevice::get_logical_device(), m_texture_image_memory, nullptr);
  }
} // namespace esp