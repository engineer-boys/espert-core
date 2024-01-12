#include "VulkanTexture.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

// #if defined(__GNUC__) && !defined(NDEBUG) && defined(__OPTIMIZE__)
// #warning "Undefing __OPTIMIZE__ because of fmt. This problem occurs after adding stb."
// #undef __OPTIMIZE__
// #endif

// // stb
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

namespace esp
{
  VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, uint32_t mip_levels) :
      EspTexture(width, height, mip_levels)
  {
  }

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
                                                               std::unique_ptr<CubemapResource> cubemap_resource,
                                                               EspTextureFormat format)
  {
    VkFormat vulkan_format = static_cast<VkFormat>(format);

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
                                                vulkan_format,
                                                vulkan_texture->m_texture_image,
                                                vulkan_texture->m_texture_image_memory);

    vulkan_texture->m_texture_image_view =
        VulkanResourceManager::create_cubemap_image_view(vulkan_texture->m_texture_image,
                                                         vulkan_format,
                                                         VK_IMAGE_ASPECT_COLOR_BIT,
                                                         vulkan_texture->m_mip_levels);

    vulkan_texture->m_sampler = VulkanSampler::create(vulkan_texture->m_mip_levels);

    return vulkan_texture;
  }

  std::shared_ptr<EspTexture> VulkanTexture::create_raw_texture(EspRawTextureParams params)
  {
    auto vulkan_texture =
        std::shared_ptr<VulkanTexture>{ new VulkanTexture(params.width, params.height, params.mip_levels) };
    auto sample_count = static_cast<VkSampleCountFlagBits>(params.sample_count);
    auto format       = static_cast<VkFormat>(params.format);

    if (params.as_cubemap)
    {
      // cubemap texture
      VulkanResourceManager::create_image(vulkan_texture->get_width(),
                                          vulkan_texture->get_height(),
                                          vulkan_texture->get_mip_levels(),
                                          sample_count,
                                          format,
                                          VK_IMAGE_TILING_OPTIMAL,
                                          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                          6,
                                          VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          vulkan_texture->m_texture_image,
                                          vulkan_texture->m_texture_image_memory);

      vulkan_texture->m_texture_image_view =
          VulkanResourceManager::create_cubemap_image_view(vulkan_texture->m_texture_image,
                                                           format,
                                                           VK_IMAGE_ASPECT_COLOR_BIT,
                                                           vulkan_texture->m_mip_levels);

      vulkan_texture->m_sampler = vulkan_texture->m_mip_levels == 1
          ? VulkanSampler::get_default_sampler()
          : VulkanSampler::create(vulkan_texture->m_mip_levels);
    }
    else
    {
      // normal texture
      VulkanResourceManager::create_image(vulkan_texture->get_width(),
                                          vulkan_texture->get_height(),
                                          vulkan_texture->get_mip_levels(),
                                          sample_count,
                                          format,
                                          VK_IMAGE_TILING_OPTIMAL,
                                          VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                          1,
                                          {},
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          vulkan_texture->m_texture_image,
                                          vulkan_texture->m_texture_image_memory);

      vulkan_texture->m_texture_image_view = VulkanResourceManager::create_image_view(vulkan_texture->m_texture_image,
                                                                                      format,
                                                                                      VK_IMAGE_ASPECT_COLOR_BIT,
                                                                                      vulkan_texture->get_mip_levels());

      vulkan_texture->m_sampler = vulkan_texture->m_mip_levels == 1
          ? VulkanSampler::get_default_sampler()
          : VulkanSampler::create(vulkan_texture->m_mip_levels);
    }

    return vulkan_texture;
  }

  std::unique_ptr<VulkanTexture> VulkanTexture::create_from_block(const VulkanBlock* block,
                                                                  bool set_as_retrived_from_block)
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
    vulkan_texture->m_retrieved_from_block = set_as_retrived_from_block;

    vulkan_texture->m_sampler = VulkanSampler::get_default_sampler();
    return vulkan_texture;
  }

  std::unique_ptr<VulkanTexture> VulkanTexture::create_from_depth_block(const esp::VulkanDepthBlock* block,
                                                                        bool set_as_retrived_from_block)
  {
    auto vulkan_texture = std::unique_ptr<VulkanTexture>(new VulkanTexture(block->get_width(), block->get_height()));

    vulkan_texture->m_texture_image        = block->get_image();
    vulkan_texture->m_texture_image_memory = block->get_image_memory();
    vulkan_texture->m_texture_image_view   = block->get_image_view();
    vulkan_texture->m_retrieved_from_block = set_as_retrived_from_block;

    vulkan_texture->m_sampler = VulkanSampler::create(0, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
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