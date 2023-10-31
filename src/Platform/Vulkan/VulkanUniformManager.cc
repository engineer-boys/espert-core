#include "VulkanUniformManager.hh"

// Platform
#include "Buffers/VulkanBuffer.hh"
#include "VulkanDevice.hh"
#include "VulkanFrameManager.hh"
#include <iostream>

static VkDescriptorSetLayoutBinding create_dslb(esp::VulkanMetaUniform& data);

namespace esp
{
  VulkanDescriptorSetPackage::DescriptorSetBuffers::DescriptorSetBuffers(std::vector<VulkanMetaUniform>& uniforms)
  {
    for (auto& uniform : uniforms)
    {
      if (uniform.m_uniform_type == EspUniformType::ESP_BUFFER_UNIFORM)
      {
        m_buffers[uniform.m_binding] = std::move(
            std::make_unique<VulkanBuffer>(uniform.m_size_of_data_chunk,
                                           uniform.m_number_of_elements,
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

        m_buffers[uniform.m_binding]->map();
      }
    }
  }

  VulkanDescriptorSetPackage::VulkanDescriptorSetPackage(VulkanUniformMetaData& meta_data,
                                                         std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
                                                         VkDescriptorPool& descriptor_pool)
  {
    for (auto& meta_ds : meta_data.m_meta_descriptor_sets)
    {
      m_buffer_groups.emplace_back(meta_ds.m_meta_uniforms);
    }

    create_descriptor_set(descriptor_set_layouts, descriptor_pool);

    int counter_ds = 0;
    for (auto& meta_ds : meta_data.m_meta_descriptor_sets)
    {
      bind_data_and_descriptors(meta_ds.m_meta_uniforms, m_descriptor_sets[counter_ds], m_buffer_groups[counter_ds]);
      counter_ds += 1;
    }
  }

  void VulkanDescriptorSetPackage::create_descriptor_set(std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
                                                         VkDescriptorPool& descriptor_pool)
  {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool     = descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    alloc_info.pSetLayouts        = descriptor_set_layouts.data();

    m_descriptor_sets.resize(descriptor_set_layouts.size());
    if (vkAllocateDescriptorSets(VulkanDevice::get_logical_device(), &alloc_info, m_descriptor_sets.data()) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate descriptor sets!");
      throw std::runtime_error("Failed to allocate descriptor sets!");
    }
  }

  void VulkanDescriptorSetPackage::bind_data_and_descriptors(std::vector<VulkanMetaUniform>& uniforms,
                                                             VkDescriptorSet& ds,
                                                             DescriptorSetBuffers& ds_data)
  {
    std::vector<VkWriteDescriptorSet> descriptor_writes;
    std::vector<VkDescriptorBufferInfo> buffer_infos;

    for (auto& uniform : uniforms)
    {
      if (uniform.m_uniform_type == EspUniformType::ESP_BUFFER_UNIFORM)
      {
        VkDescriptorBufferInfo buffer_info{};
        buffer_info.buffer = ds_data.m_buffers[uniform.m_binding]->get_buffer();
        buffer_info.offset = 0;
        buffer_info.range  = uniform.m_size_of_data_chunk * uniform.m_number_of_elements;
        buffer_infos.push_back(buffer_info);

        VkWriteDescriptorSet descriptor_write{};
        descriptor_write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstSet          = ds;
        descriptor_write.dstBinding      = uniform.m_binding;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write.descriptorCount = uniform.m_number_of_elements;
        descriptor_write.pBufferInfo     = &buffer_infos.back();

        descriptor_writes.push_back(descriptor_write);
      }
      else if (uniform.m_uniform_type == EspUniformType::ESP_TEXTURE)
      {
        ESP_CORE_ERROR("NO IMPLEMENTED TEXTURES!");
        throw std::runtime_error("NO IMPLEMENTED TEXTURES!");
      }
    }

    vkUpdateDescriptorSets(VulkanDevice::get_logical_device(),
                           static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(),
                           0,
                           nullptr);
  }

  void VulkanDescriptorSetPackage::write(int set, int binding, int size, void* data)
  {
    m_buffer_groups[set].m_buffers[binding]->write_to_buffer(data, size);
  }

  void VulkanDescriptorSetPackage::write(int set, int binding, int elem, int size, void* data)
  {
    m_buffer_groups[set].m_buffers[binding]->write_to_buffer(data, size, elem * size);
  }

} // namespace esp

namespace esp
{
  VulkanUniformManager::VulkanUniformManager(std::unique_ptr<VulkanUniformMetaData> meta_data) :
      m_meta_data{ std::move(meta_data) }
  {
    /*
      SHORTCUTS:
        dsl - Descriptor Set Layout
    */

    int counter_dsl = 1;
    for (auto& meta_dsl : m_meta_data->m_meta_descriptor_sets)
    {
      std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings;

      for (auto& meta_uniform : meta_dsl.m_meta_uniforms)
      {
        descriptor_set_layout_bindings.push_back(create_dslb(meta_uniform));
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
      m_ds_layouts.push_back(descriptor_set_layout);

      counter_dsl += 1;
    }

    init_descriptor_pool();
    init_descriptor_sets();
  }

  VulkanUniformManager::~VulkanUniformManager()
  {
    for (auto& dsl : m_ds_layouts)
    {
      vkDestroyDescriptorSetLayout(VulkanDevice::get_logical_device(), dsl, nullptr);
    }

    vkDestroyDescriptorPool(VulkanDevice::get_logical_device(), m_descriptor_pool, nullptr);
  }

  void VulkanUniformManager::init_descriptor_pool()
  {
    std::vector<VkDescriptorPoolSize> pool_sizes;

    if (m_meta_data->m_buffer_uniform_counter != 0)
    {
      VkDescriptorPoolSize pool_size{};
      pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      pool_size.descriptorCount =
          static_cast<uint32_t>(m_meta_data->m_buffer_uniform_counter * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

      pool_sizes.push_back(pool_size);
    }

    if (m_meta_data->m_texture_uniform_counter != 0)
    {
      VkDescriptorPoolSize pool_size{};
      pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      pool_size.descriptorCount =
          static_cast<uint32_t>(m_meta_data->m_texture_uniform_counter * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

      pool_sizes.push_back(pool_size);
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes    = pool_sizes.data();
    pool_info.maxSets =
        static_cast<uint32_t>((m_meta_data->m_buffer_uniform_counter + m_meta_data->m_texture_uniform_counter) *
                              VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(VulkanDevice::get_logical_device(), &pool_info, nullptr, &m_descriptor_pool) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create descriptor pool!");
      throw std::runtime_error("Failed to create descriptor pool!");
    }
  }

  void VulkanUniformManager::init_descriptor_sets()
  {
    for (int frame_idx = 0; frame_idx < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; frame_idx++)
    {
      m_descriptor_set_packages.emplace_back(*m_meta_data, m_ds_layouts, m_descriptor_pool);
    }
  }

  void VulkanUniformManager::write(int set, int binding, int size, void* data)
  {
    m_descriptor_set_packages[VulkanFrameManager::get_current_frame_index()].write(set, binding, size, data);
  }

  void VulkanUniformManager::write(int set, int binding, int elem, int size, void* data)
  {
    m_descriptor_set_packages[VulkanFrameManager::get_current_frame_index()].write(set, binding, elem, size, data);
  }

  void VulkanUniformManager::attach_descriptor_set_package(const VkPipelineLayout& pipeline_layout) const
  {
    // m_descriptor_sets
    vkCmdBindDescriptorSets(VulkanFrameManager::get_current_command_buffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline_layout,
                            0,
                            m_descriptor_set_packages[VulkanFrameManager::get_current_frame_index()].count_ds(),
                            m_descriptor_set_packages[VulkanFrameManager::get_current_frame_index()].get_ds().data(),
                            0,
                            nullptr);
  }

} // namespace esp

static VkDescriptorSetLayoutBinding create_dslb(esp::VulkanMetaUniform& data)
{
  auto stage = VK_SHADER_STAGE_VERTEX_BIT;
  if (data.m_stage == esp::EspUniformShaderStage::ESP_FRAG_STAGE) { stage = VK_SHADER_STAGE_FRAGMENT_BIT; }

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

  case esp::EspUniformType::ESP_SMALL_FAST_UNIFORM:
    /* code */
    throw std::runtime_error("Not implemented ESP_SMALL_FAST_UNIFORM!");
    break;

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
