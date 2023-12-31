#ifndef VULKAN_RENDER_API_VULKAN_TEXTURE_HH
#define VULKAN_RENDER_API_VULKAN_TEXTURE_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "VulkanSampler.hh"

// Core
#include "Core/Resources/Systems/TextureSystem.hh"

// Render API Vulkan
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanBlock.hh"

namespace esp
{
  class VulkanTexture : public EspTexture
  {
   private:
    // true - if VulkanTexture is from EspBlock. Otherwise, it is false.
    bool m_retrieved_from_block = false;

    VkImage m_texture_image;
    VkDeviceMemory m_texture_image_memory;
    VkImageView m_texture_image_view;

    std::shared_ptr<VulkanSampler> m_sampler;

   public:
    static std::shared_ptr<VulkanTexture> create(const std::string name,
                                                 std::unique_ptr<ImageResource> image,
                                                 EspTextureType type = EspTextureType::ALBEDO,
                                                 bool mipmapping     = false);

    static std::shared_ptr<VulkanTexture> create_cubemap(const std::string name,
                                                         std::unique_ptr<CubemapResource> cubemap_resource);

    static std::unique_ptr<VulkanTexture> create_from_block(const VulkanBlock* block);

    PREVENT_COPY(VulkanTexture);

    ~VulkanTexture();

    inline VkImage get_texture_image() { return m_texture_image; }
    inline VkDeviceMemory get_texture_image_memory() { return m_texture_image_memory; }
    inline VkImageView get_texture_image_view() { return m_texture_image_view; }
    inline VkSampler get_sampler() { return m_sampler->get_sampler(); }

   private:
    VulkanTexture(const std::string& name,
                  const uint8_t* pixels,
                  uint8_t channel_count,
                  uint32_t width,
                  uint32_t height,
                  EspTextureType type = EspTextureType::ALBEDO);

    VulkanTexture(const std::string& name, uint8_t channel_count, uint32_t width, uint32_t height);

    VulkanTexture(uint32_t width, uint32_t height);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_TEXTURE_HH
