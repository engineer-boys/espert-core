#ifndef VULKAN_RENDER_API_VULKAN_TEXTURE_HH
#define VULKAN_RENDER_API_VULKAN_TEXTURE_HH

#include "esppch.hh"

// Render API
#include "VulkanSampler.hh"

// Core
#include "Core/Systems/TextureSystem.hh"

namespace esp
{
  class VulkanTexture
  {
   private:
    VkImage m_texture_image;
    VkDeviceMemory m_texture_image_memory;
    VkImageView m_texture_image_view;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_mip_levels;

    std::shared_ptr<VulkanSampler> m_sampler;

   public:
    static std::unique_ptr<VulkanTexture> create(const std::shared_ptr<Texture> texture, bool mipmapping = false);

    VulkanTexture(const VulkanTexture&)            = delete;
    VulkanTexture& operator=(const VulkanTexture&) = delete;

    ~VulkanTexture();

    inline VkImage get_texture_image() { return m_texture_image; }
    inline VkDeviceMemory get_texture_image_memory() { return m_texture_image_memory; }
    inline VkImageView get_texture_image_view() { return m_texture_image_view; }
    inline VkSampler get_sampler() { return m_sampler->get_sampler(); }

   private:
    VulkanTexture() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_TEXTURE_HH
