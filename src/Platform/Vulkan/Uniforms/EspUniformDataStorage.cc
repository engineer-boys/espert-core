#include "EspUniformDataStorage.hh"

// platform
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanSwapChain.hh"

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
      //   std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings;

      //   for (auto& meta_uniform : meta_dsl.m_meta_uniforms)
      //   {
      //     descriptor_set_layout_bindings.push_back(create_descriptor_set_layout_binding(meta_uniform));
      //   }

      //   VkDescriptorSetLayoutCreateInfo layout_info{};
      //   layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      //   layout_info.bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings.size());
      //   layout_info.pBindings    = descriptor_set_layout_bindings.data();

      //   VkDescriptorSetLayout descriptor_set_layout;
      //   if (vkCreateDescriptorSetLayout(VulkanDevice::get_logical_device(),
      //                                   &layout_info,
      //                                   nullptr,
      //                                   &descriptor_set_layout) != VK_SUCCESS)
      //   {
      //     ESP_CORE_ERROR("Failed to create descriptor set layout!");
      //     throw std::runtime_error("Failed to create descriptor set layout!");
      //   }
      //   m_descriptor_set_layouts.push_back(descriptor_set_layout);
      m_descriptor_set_layouts.push_back(create_descriptor_set_layout(meta_dsl.first, meta_dsl.second));
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

// static VkDescriptorSetLayoutBinding create_descriptor_set_layout_binding(esp::EspMetaUniform& data)
// {
//   auto stage = VK_SHADER_STAGE_VERTEX_BIT;
//   if (data.m_stage == esp::EspUniformShaderStage::ESP_FRAG_STAGE) { stage = VK_SHADER_STAGE_FRAGMENT_BIT; }
//   else if (data.m_stage == esp::EspUniformShaderStage::ESP_ALL_STAGES) { stage = VK_SHADER_STAGE_ALL_GRAPHICS; }

//   switch (data.m_uniform_type)
//   {
//   case esp::EspUniformType::ESP_BUFFER_UNIFORM:
//   {
//     VkDescriptorSetLayoutBinding ubo_layout_binding{};
//     ubo_layout_binding.binding            = data.m_binding;
//     ubo_layout_binding.descriptorCount    = data.m_number_of_elements;
//     ubo_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//     ubo_layout_binding.pImmutableSamplers = nullptr;
//     ubo_layout_binding.stageFlags         = stage;
//     return ubo_layout_binding;
//   }

//   case esp::EspUniformType::ESP_SMALL_FAST_UNIFORM:
//     /* code */
//     throw std::runtime_error("Not implemented ESP_SMALL_FAST_UNIFORM!");
//     break;

//   case esp::EspUniformType::ESP_TEXTURE:
//   {
//     VkDescriptorSetLayoutBinding sampler_layout_binding{};
//     sampler_layout_binding.binding            = data.m_binding;
//     sampler_layout_binding.descriptorCount    = data.m_number_of_elements;
//     sampler_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//     sampler_layout_binding.pImmutableSamplers = nullptr;
//     sampler_layout_binding.stageFlags         = stage;
//     return sampler_layout_binding;
//   }

//   default:
//     ESP_CORE_ERROR("Given uniform type doesn't exist!");
//     throw std::runtime_error("Given uniform type doesn't exist!");
//     break;
//   }
// }

static VkDescriptorType find_descriptor_type(esp::ShaderPropertyType property_type, bool dynamic)
{
  switch (property_type)
  {
  case esp::ShaderPropertyType::InputAttachment:
    return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    break;
  case esp::ShaderPropertyType::Image:
    return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    break;
  case esp::ShaderPropertyType::ImageSampler:
    return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    break;
  case esp::ShaderPropertyType::ImageStorage:
    return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    break;
  case esp::ShaderPropertyType::Sampler:
    return VK_DESCRIPTOR_TYPE_SAMPLER;
    break;
  case esp::ShaderPropertyType::BufferUniform:
    if (dynamic) { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; }
    else { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
    break;
  case esp::ShaderPropertyType::BufferStorage:
    if (dynamic) { return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC; }
    else { return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }
    break;
  default:
    throw std::runtime_error("No conversion possible for the shader resource type.");
    break;
  }
}

static bool validate_binding(const VkDescriptorSetLayoutBinding& binding,
                             const std::vector<VkDescriptorType>& blacklist)
{
  return !(std::find_if(blacklist.begin(),
                        blacklist.end(),
                        [binding](const VkDescriptorType& type)
                        { return type == binding.descriptorType; }) != blacklist.end());
}

static bool validate_flags(const std::vector<VkDescriptorSetLayoutBinding>& bindings,
                           const std::vector<VkDescriptorBindingFlagsEXT>& flags)
{
  if (flags.empty()) { return true; }

  if (bindings.size() != flags.size())
  {
    ESP_CORE_ERROR("Binding count has to be equal to flag count.");
    return false;
  }

  return true;
}

static VkDescriptorSetLayout create_descriptor_set_layout(const uint32_t set_index,
                                                          const esp::EspMetaDescriptorSet& meta_descriptor_set)
{
  std::vector<VkDescriptorSetLayoutBinding> bindings;
  std::vector<VkDescriptorBindingFlagsEXT> binding_flags;

  for (auto& property : meta_descriptor_set)
  {
    if (property.type == esp::ShaderPropertyType::Input || property.type == esp::ShaderPropertyType::Output ||
        property.type == esp::ShaderPropertyType::PushConstant ||
        property.type == esp::ShaderPropertyType::SpecializationConstant)
    {
      continue;
    }

    auto descriptor_type = find_descriptor_type(property.type, property.mode == esp::ShaderPropertyMode::Dynamic);

    if (property.mode == esp::ShaderPropertyMode::UpdateAfterBind)
    {
      binding_flags.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT);
    }
    else { binding_flags.push_back(0); }

    VkDescriptorSetLayoutBinding layout_binding{};

    layout_binding.binding         = property.binding;
    layout_binding.descriptorCount = property.array_size;
    layout_binding.descriptorType  = descriptor_type;
    layout_binding.stageFlags      = static_cast<VkShaderStageFlags>(property.stages);

    bindings.push_back(layout_binding);
  }

  VkDescriptorSetLayoutCreateInfo create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
  create_info.flags        = 0;
  create_info.bindingCount = static_cast<uint32_t>(bindings.size());
  create_info.pBindings    = bindings.data();

  if (std::find_if(meta_descriptor_set.begin(),
                   meta_descriptor_set.end(),
                   [](const esp::ShaderProperty& shader_property) {
                     return shader_property.mode == esp::ShaderPropertyMode::UpdateAfterBind;
                   }) != meta_descriptor_set.end())
  {
    if (std::find_if(meta_descriptor_set.begin(),
                     meta_descriptor_set.end(),
                     [](const esp::ShaderProperty& shader_property)
                     { return shader_property.mode == esp::ShaderPropertyMode::Dynamic; }) != meta_descriptor_set.end())
    {
      throw std::runtime_error("Cannot create descriptor set layout, dynamic resources are not allowed if at least one "
                               "resource is update-after-bind.");
    }

    if (!validate_flags(bindings, binding_flags))
    {
      throw std::runtime_error("Invalid binding, couldn't create descriptor set layout.");
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT binding_flags_create_info{
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT
    };
    binding_flags_create_info.bindingCount  = static_cast<uint32_t>(binding_flags.size());
    binding_flags_create_info.pBindingFlags = binding_flags.data();

    create_info.pNext = &binding_flags_create_info;
    create_info.flags |=
        std::find(binding_flags.begin(), binding_flags.end(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT) !=
            binding_flags.end()
        ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT
        : 0;
  }

  VkDescriptorSetLayout descriptor_set_layout;
  if (vkCreateDescriptorSetLayout(esp::VulkanDevice::get_logical_device(),
                                  &create_info,
                                  nullptr,
                                  &descriptor_set_layout) != VK_SUCCESS)
  {
    ESP_CORE_ERROR("Failed to create desscriptor set layout.");
    throw std::runtime_error("Failed to create desscriptor set layout.");
  }

  return descriptor_set_layout;
}
