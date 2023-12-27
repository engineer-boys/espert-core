#ifndef VULKAN_RENDER_API_VULKAN_CUBEMAP_HH
#define VULKAN_RENDER_API_VULKAN_CUBEMAP_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Resources/EspCubemap.hh"
#include "Platform/Vulkan/Resources/VulkanSampler.hh"

// Core
#include "Core/Resources/Systems/TextureSystem.hh"

namespace esp
{
  class VulkanCubemap : public EspCubemap
  {
   private:
    static const uint8_t s_num_of_faces = 6;
    VkImage m_cubemap_image;
    VkDeviceMemory m_image_memory;
    VkImageView m_image_view;

    std::shared_ptr<VulkanSampler> m_sampler;

   public:
    static std::shared_ptr<VulkanCubemap> create(const std::string name,
                                                 std::unique_ptr<CubemapResource> cubemap_resource);

    PREVENT_COPY(VulkanCubemap);

    ~VulkanCubemap();

    inline VkDeviceMemory get_texture_image_memory() { return m_image_memory; }
    inline VkImageView get_texture_image_view() { return m_image_view; }
    inline VkSampler get_sampler() { return m_sampler->get_sampler(); }

   private:
    VulkanCubemap(const std::string& name, uint8_t channel_count, uint32_t width, uint32_t height);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_CUBEMAP_HH
