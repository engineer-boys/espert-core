#ifndef VULKAN_RENDER_API_VULKAN_TEXTURE_HH
#define VULKAN_RENDER_API_VULKAN_TEXTURE_HH

#include "esppch.hh"

namespace esp
{
  class VulkanTexture
  {
   private:
    VkImage m_texture_image;
    VkDeviceMemory m_texture_image_memory;
    VkImageView m_texture_image_view;

   public:
    static std::unique_ptr<VulkanTexture> create(const std::string& path);

    VulkanTexture(const VulkanTexture&)            = delete;
    VulkanTexture& operator=(const VulkanTexture&) = delete;

    ~VulkanTexture();

    inline VkImage get_texture_image() { return m_texture_image; }
    inline VkDeviceMemory get_texture_image_memory() { return m_texture_image_memory; }
    inline VkImageView get_texture_image_view() { return m_texture_image_view; }

   private:
    VulkanTexture() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_TEXTURE_HH
