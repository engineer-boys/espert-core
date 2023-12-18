#include "VulkanCubemap.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

namespace esp
{
  const uint8_t VulkanCubemap::s_num_of_faces;

  VulkanCubemap::VulkanCubemap(const std::string& name, uint8_t channel_count, uint32_t width, uint32_t height) :
      EspCubemap(name, channel_count, width, height)
  {
  }

  std::shared_ptr<VulkanCubemap> VulkanCubemap::create(const std::string name,
                                                       std::unique_ptr<CubemapResource> cubemap_resource)
  {
    auto vulkan_cubemap = std::shared_ptr<VulkanCubemap>(
        new VulkanCubemap(name,
                          cubemap_resource->get_face(EspCubemapFace::TOP).get_channel_count(),
                          cubemap_resource->get_face(EspCubemapFace::TOP).get_width(),
                          cubemap_resource->get_face(EspCubemapFace::TOP).get_height()));

    std::array<const void*, 6> data;
    int i = 0;
    for (auto face = EspCubemapFace::TOP; face < EspCubemapFace::ENUM_END; ++face)
    {
      data[i++] = cubemap_resource->get_face(EspCubemapFace::TOP).get_data();
    }

    VulkanResourceManager::create_cubemap_image(vulkan_cubemap->get_width(),
                                                vulkan_cubemap->get_height(),
                                                data.data(),
                                                vulkan_cubemap->get_mip_levels(),
                                                vulkan_cubemap->m_cubemap_image,
                                                vulkan_cubemap->m_image_memory);

    vulkan_cubemap->m_image_view = VulkanResourceManager::create_cubemap_image_view(vulkan_cubemap->m_cubemap_image,
                                                                                    VK_FORMAT_R8G8B8A8_SRGB,
                                                                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                                                                    vulkan_cubemap->m_mip_levels);

    vulkan_cubemap->m_sampler = VulkanSampler::create(vulkan_cubemap->m_mip_levels);

    return vulkan_cubemap;
  }

  VulkanCubemap::~VulkanCubemap()
  {
    vkDestroyImageView(VulkanDevice::get_logical_device(), m_image_view, nullptr);
    vkDestroyImage(VulkanDevice::get_logical_device(), m_cubemap_image, nullptr);
    vkFreeMemory(VulkanDevice::get_logical_device(), m_image_memory, nullptr);
  }
} // namespace esp