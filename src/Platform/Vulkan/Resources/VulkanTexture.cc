#include "VulkanTexture.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

namespace esp
{
  VulkanTexture::VulkanTexture(uint32_t width, uint32_t height) : EspTexture(width, height) {}

  VulkanTexture::VulkanTexture(const std::string& name,
                               const uint8_t* pixels,
                               uint8_t channel_count,
                               uint32_t width,
                               uint32_t height,
                               EspTextureType type) :
      EspTexture(name, pixels, channel_count, width, height, type)
  {
  }

  VulkanTexture::VulkanTexture(const std::string& name, uint8_t channel_count, uint32_t width, uint32_t height) :
      EspTexture(name, channel_count, width, height)
  {
  }

  std::shared_ptr<VulkanTexture> VulkanTexture::create(const std::string name,
                                                       std::unique_ptr<ImageResource> image,
                                                       EspTextureType type,
                                                       bool mipmapping,
                                                       EspTextureFormat format)
  {
    VkFormat vulkan_format = static_cast<VkFormat>(format);

    auto vulkan_texture =
        std::shared_ptr<VulkanTexture>(new VulkanTexture(name,
                                                         static_cast<const uint8_t*>(image->get_data()),
                                                         image->get_channel_count(),
                                                         image->get_width(),
                                                         image->get_height(),
                                                         type));

    VulkanResourceManager::create_texture_image(vulkan_texture->get_width(),
                                                vulkan_texture->get_height(),
                                                image->release(),
                                                vulkan_texture->get_mip_levels(),
                                                vulkan_format,
                                                vulkan_texture->m_texture_image,
                                                vulkan_texture->m_texture_image_memory);

    vulkan_texture->m_texture_image_view = VulkanResourceManager::create_image_view(vulkan_texture->m_texture_image,
                                                                                    vulkan_format,
                                                                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                                                                    vulkan_texture->m_mip_levels);

    vulkan_texture->m_sampler =
        !mipmapping ? VulkanSampler::get_default_sampler() : VulkanSampler::create(vulkan_texture->m_mip_levels);

    return vulkan_texture;
  }

  std::shared_ptr<VulkanTexture> VulkanTexture::create_cubemap(const std::string name,
                                                               std::unique_ptr<CubemapResource> cubemap_resource)
  {
    auto vulkan_texture = std::shared_ptr<VulkanTexture>(
        new VulkanTexture(name,
                          cubemap_resource->get_face(EspCubemapFace::RIGHT).get_channel_count(),
                          cubemap_resource->get_face(EspCubemapFace::RIGHT).get_width(),
                          cubemap_resource->get_face(EspCubemapFace::RIGHT).get_height()));

    std::array<const void*, 6> data;
    int i = 0;
    for (auto face = EspCubemapFace::RIGHT; face < EspCubemapFace::ENUM_END; ++face)
    {
      data[i++] = cubemap_resource->get_face(face).get_data();
    }

    VulkanResourceManager::create_cubemap_image(vulkan_texture->get_width(),
                                                vulkan_texture->get_height(),
                                                data.data(),
                                                vulkan_texture->get_mip_levels(), // 1
                                                vulkan_texture->m_texture_image,
                                                vulkan_texture->m_texture_image_memory);

    vulkan_texture->m_texture_image_view =
        VulkanResourceManager::create_cubemap_image_view(vulkan_texture->m_texture_image,
                                                         VK_FORMAT_R8G8B8A8_SRGB,
                                                         VK_IMAGE_ASPECT_COLOR_BIT,
                                                         vulkan_texture->m_mip_levels);

    vulkan_texture->m_sampler = VulkanSampler::create(vulkan_texture->m_mip_levels);

    return vulkan_texture;
  }

  std::unique_ptr<VulkanTexture> VulkanTexture::create_from_block(const VulkanBlock* block)
  {
    auto vulkan_texture = std::unique_ptr<VulkanTexture>(new VulkanTexture(block->get_width(), block->get_height()));

    if (block->is_resolvable())
    {
      vulkan_texture->m_texture_image        = block->get_resolve_image();
      vulkan_texture->m_texture_image_memory = block->get_resolve_image_memory();
      vulkan_texture->m_texture_image_view   = block->get_resolve_image_view();
    }
    else
    {
      vulkan_texture->m_texture_image        = block->get_image();
      vulkan_texture->m_texture_image_memory = block->get_image_memory();
      vulkan_texture->m_texture_image_view   = block->get_image_view();
    }
    vulkan_texture->m_retrieved_from_block = true;

    vulkan_texture->m_sampler = VulkanSampler::get_default_sampler();
    return vulkan_texture;
  }

  VulkanTexture::~VulkanTexture()
  {
    if (!m_retrieved_from_block)
    {
      vkDestroyImageView(VulkanDevice::get_logical_device(), m_texture_image_view, nullptr);
      vkDestroyImage(VulkanDevice::get_logical_device(), m_texture_image, nullptr);
      vkFreeMemory(VulkanDevice::get_logical_device(), m_texture_image_memory, nullptr);
    }
  }
} // namespace esp