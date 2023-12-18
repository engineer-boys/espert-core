#include "VulkanSampler.hh"
#include "Platform/Vulkan/VulkanDevice.hh"

namespace esp
{
  std::shared_ptr<VulkanSampler> VulkanSampler::s_default_sampler = nullptr;

  std::shared_ptr<VulkanSampler> VulkanSampler::create(uint32_t mip_levels)
  {
    return std::make_shared<VulkanSampler>(mip_levels);
  }

  VulkanSampler::VulkanSampler(uint32_t mip_levels)
  {
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter               = VK_FILTER_LINEAR;
    sampler_info.minFilter               = VK_FILTER_LINEAR;
    sampler_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable        = VK_TRUE;
    sampler_info.maxAnisotropy           = VulkanDevice::get_properties().limits.maxSamplerAnisotropy;
    sampler_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable           = VK_FALSE;
    sampler_info.compareOp               = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias              = 0.0f;
    sampler_info.minLod                  = 0;
    sampler_info.maxLod                  = static_cast<float>(mip_levels);

    if (vkCreateSampler(VulkanDevice::get_logical_device(), &sampler_info, nullptr, &m_sampler) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create texture sampler");
      throw std::runtime_error("Failed to create texture sampler");
    }
  }

  VulkanSampler::~VulkanSampler()
  {
    if (this != s_default_sampler.get()) { vkDestroySampler(VulkanDevice::get_logical_device(), m_sampler, nullptr); }
  }

  void VulkanSampler::create_default_sampler() { s_default_sampler = std::make_shared<VulkanSampler>(0); }

  void VulkanSampler::terminate_default_sampler()
  {
    vkDestroySampler(VulkanDevice::get_logical_device(), s_default_sampler->m_sampler, nullptr);
  }

} // namespace esp