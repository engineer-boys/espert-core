#ifndef VULKAN_RENDER_API_VULKAN_RESOURCE_MANAGER_HH
#define VULKAN_RENDER_API_VULKAN_RESOURCE_MANAGER_HH

#include "esppch.hh"

namespace esp
{
  class VulkanResourceManager
  {
   private:
    static VulkanResourceManager* s_instance;

   public:
    static std::unique_ptr<VulkanResourceManager> create();

    VulkanResourceManager(const VulkanResourceManager&)            = delete;
    VulkanResourceManager& operator=(const VulkanResourceManager&) = delete;

    ~VulkanResourceManager();

    void terminate();

    // ---------------------------------------- Buffer Helper Functions ----------------------------------------
    static void allocate_buffer_on_device(VkDeviceSize size,
                                          VkBufferUsageFlags usage,
                                          VkMemoryPropertyFlags properties,
                                          VkBuffer& buffer,
                                          VkDeviceMemory& buffer_memory);

    static void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
    // ---------------------------------------------------------------------------------------------------------

    // ---------------------------------------- Image Helper Functions ----------------------------------------
    static void copy_buffer_to_image(VkBuffer buffer,
                                     VkImage image,
                                     uint32_t width,
                                     uint32_t height,
                                     uint32_t layer_count);

    static void create_image(uint32_t width,
                             uint32_t height,
                             uint32_t mip_levels,
                             VkSampleCountFlagBits num_samples,
                             VkFormat format,
                             VkImageTiling tiling,
                             VkImageUsageFlags usage,
                             VkMemoryPropertyFlags properties,
                             VkImage& image,
                             VkDeviceMemory& image_memory);

    static VkImageView create_image_view(VkImage image,
                                         VkFormat format,
                                         VkImageAspectFlags aspect_flags,
                                         uint32_t mip_levels);

    static void create_texture_image(const std::string& path,
                                     uint32_t& texture_width,
                                     uint32_t& texture_height,
                                     uint32_t& texture_mip_levels,
                                     VkImage& texture_image,
                                     VkDeviceMemory& texture_image_memory);

    static void transition_image_layout(VkImage image,
                                        VkFormat format,
                                        VkImageLayout old_layout,
                                        VkImageLayout new_layout,
                                        uint32_t mip_levels);

    static void generate_mipmaps(VkImage image,
                                 VkFormat format,
                                 uint32_t texture_width,
                                 uint32_t texture_height,
                                 uint32_t mip_levels);
    // ---------------------------------------------------------------------------------------------------------

   private:
    VulkanResourceManager();
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_RESOURCE_MANAGER_HH
