#include "EspUniformDataStorage.hh"

// platform
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanSwapChain.hh"

static VkDescriptorSetLayoutBinding create_descriptor_set_layout_binding(esp::EspMetaUniform& data);

/* --------------------------------------------------------- */
/* ----------------- EspUniformDataStorage ----------------- */
/* --------------------------------------------------------- */

namespace esp
{
  EspUniformDataStorage::EspUniformDataStorage(std::unique_ptr<VulkanUniformMetaData> meta_data) :
      m_meta_data{ std::move(meta_data) }
  {
    /*
     SHORTCUTS:
       dsl - Descriptor Set Layout
   */

    for (auto& meta_dsl : m_meta_data->m_meta_descriptor_sets)
    {
      std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings;

      for (auto& meta_uniform : meta_dsl.m_meta_uniforms)
      {
        descriptor_set_layout_bindings.push_back(create_descriptor_set_layout_binding(meta_uniform));
      }

      VkDescriptorSetLayoutCreateInfo layout_info{};
      layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      layout_info.bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings.size());
      layout_info.pBindings    = descriptor_set_layout_bindings.data();

      VkDescriptorSetLayout descriptor_set_layout;
      if (vkCreateDescriptorSetLayout(VulkanDevice::get_logical_device(),
                                      &layout_info,
                                      nullptr,
                                      &descriptor_set_layout) != VK_SUCCESS)
      {
        ESP_CORE_ERROR("Failed to create descriptor set layout!");
        throw std::runtime_error("Failed to create descriptor set layout!");
      }
      m_descriptor_set_layouts.push_back(descriptor_set_layout);
    }

    for (auto& push : m_meta_data->m_meta_pushes)
    {
      auto stage = VK_SHADER_STAGE_VERTEX_BIT;
      if (push.m_stage == esp::EspUniformShaderStage::ESP_FRAG_STAGE) { stage = VK_SHADER_STAGE_FRAGMENT_BIT; }
      else if (push.m_stage == esp::EspUniformShaderStage::ESP_ALL_STAGES) { stage = VK_SHADER_STAGE_ALL_GRAPHICS; }

      VkPushConstantRange push_constant_range{};
      push_constant_range.stageFlags = stage;
      push_constant_range.offset     = push.m_offset;
      push_constant_range.size       = push.m_size;

      m_push_constant_ranges.push_back(push_constant_range);
    }
  }

  EspUniformDataStorage::~EspUniformDataStorage()
  {
    for (auto& dsl : m_descriptor_set_layouts)
    {
      vkDestroyDescriptorSetLayout(VulkanDevice::get_logical_device(), dsl, nullptr);
    }
  }
} // namespace esp

/* --------------------------------------------------------- */
/* ------------------- HELPFUL FUNTION --------------------- */
/* --------------------------------------------------------- */

static VkDescriptorSetLayoutBinding create_descriptor_set_layout_binding(esp::EspMetaUniform& data)
{
  auto stage = VK_SHADER_STAGE_VERTEX_BIT;
  if (data.m_stage == esp::EspUniformShaderStage::ESP_FRAG_STAGE) { stage = VK_SHADER_STAGE_FRAGMENT_BIT; }
  else if (data.m_stage == esp::EspUniformShaderStage::ESP_ALL_STAGES) { stage = VK_SHADER_STAGE_ALL_GRAPHICS; }

  switch (data.m_uniform_type)
  {
  case esp::EspUniformType::ESP_BUFFER_UNIFORM:
  {
    VkDescriptorSetLayoutBinding ubo_layout_binding{};
    ubo_layout_binding.binding            = data.m_binding;
    ubo_layout_binding.descriptorCount    = data.m_number_of_elements;
    ubo_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_layout_binding.pImmutableSamplers = nullptr;
    ubo_layout_binding.stageFlags         = stage;
    return ubo_layout_binding;
  }
  case esp::EspUniformType::ESP_TEXTURE:
  {
    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding            = data.m_binding;
    sampler_layout_binding.descriptorCount    = data.m_number_of_elements;
    sampler_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags         = stage;
    return sampler_layout_binding;
  }

  default:
    ESP_CORE_ERROR("Given uniform type doesn't exist!");
    throw std::runtime_error("Given uniform type doesn't exist!");
    break;
  }
}
