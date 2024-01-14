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
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanDepthBlock.hh"

namespace esp
{
  /// @brief Texture stored on the GPU with Vulkan.
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
    /// @brief Creates texture with all necessary components.
    /// @param name Texture name.
    /// @param image Image resource containing pixel data.
    /// @param type Type of texture.
    /// @param mipmapping Bool indicating if texture uses mipmaps.
    /// @param format Format of textures pixel data.
    /// @return Shared pointer to instance of texture.
    static std::shared_ptr<VulkanTexture> create(const std::string name,
                                                 std::unique_ptr<ImageResource> image,
                                                 EspTextureType type     = EspTextureType::ALBEDO,
                                                 bool mipmapping         = false,
                                                 EspTextureFormat format = EspTextureFormat::ESP_FORMAT_R8G8B8A8_SRGB);

    /// @brief Creates cubemap texture with all necessary components.
    /// @param name Cubemap name.
    /// @param cubemap_resource Cubemap resource containing pixel data for all 6 faces.
    /// @param format Format of cubemap pixel data.
    /// @return Shared pointer to instance of cubemap.
    static std::shared_ptr<VulkanTexture> create_cubemap(const std::string name,
                                                         std::unique_ptr<CubemapResource> cubemap_resource,
                                                         EspTextureFormat format);

    static std::shared_ptr<EspTexture> create_raw_texture(EspRawTextureParams params);

    static std::unique_ptr<VulkanTexture> create_from_block(const VulkanBlock* block,
                                                            bool set_as_retrived_from_block = true);
    static std::unique_ptr<VulkanTexture> create_from_depth_block(const VulkanDepthBlock* block,
                                                                  bool set_as_retrived_from_block = true);

    PREVENT_COPY(VulkanTexture);

    /// @brief Destroys Vulkan's GPU objects.
    ~VulkanTexture();

    /// @brief Returns Vulkan's texture image.
    /// @return Vulkan's texture image.
    inline VkImage get_texture_image() { return m_texture_image; }
    /// @brief Returns Vulkan's texture image memory.
    /// @return Vulkan's texture image memory.
    inline VkDeviceMemory get_texture_image_memory() { return m_texture_image_memory; }
    /// @brief Returns Vulkan's texture image view.
    /// @return Vulkan's texture image view.
    inline VkImageView get_texture_image_view() { return m_texture_image_view; }
    /// @brief Returns Vulkan's texture sampler.
    /// @return Vulkan's texture sampler.
    inline VkSampler get_sampler() { return m_sampler->get_sampler(); }

   private:
    VulkanTexture(const std::string& name,
                  const uint8_t* pixels,
                  uint8_t channel_count,
                  uint32_t width,
                  uint32_t height,
                  EspTextureType type = EspTextureType::ALBEDO);

    VulkanTexture(const std::string& name, uint8_t channel_count, uint32_t width, uint32_t height);

    VulkanTexture(uint32_t width, uint32_t height, uint32_t mip_levels = 1);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_TEXTURE_HH
