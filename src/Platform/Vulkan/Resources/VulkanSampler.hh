#ifndef VULKAN_RENDER_API_VULKAN_SAMPLER_HH
#define VULKAN_RENDER_API_VULKAN_SAMPLER_HH

#include "esppch.hh"

// Render API
#include "Platform/Vulkan/VulkanContext.hh"

namespace esp
{
  class VulkanSampler
  {
   private:
    static std::shared_ptr<VulkanSampler> s_default_sampler;

    VkSampler m_sampler;

   public:
    static std::shared_ptr<VulkanSampler> create(uint32_t mip_levels               = 0,
                                                 VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

    VulkanSampler(uint32_t mip_levels = 0, VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

    VulkanSampler(const VulkanSampler&)            = delete;
    VulkanSampler& operator=(const VulkanSampler&) = delete;

    ~VulkanSampler();

    inline VkSampler get_sampler() { return m_sampler; }
    inline static std::shared_ptr<VulkanSampler> get_default_sampler() { return s_default_sampler; }

   private:
    static void create_default_sampler();
    static void terminate_default_sampler();

    friend class VulkanContext;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_SAMPLER_HH
