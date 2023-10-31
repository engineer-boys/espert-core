#include "VulkanUniformManager.hh"

// Platform
#include "Buffers/VulkanBuffer.hh"
#include "VulkanDevice.hh"
#include "VulkanFrameManager.hh"
#include <iostream>

static VkDescriptorSetLayoutBinding create_dslb(esp::VulkanMetaUniform& data);

namespace esp
{
  VulkanDescriptorSetPacgake::DescriptorSetBuffers::DescriptorSetBuffers(std::vector<VulkanMetaUniform>& uniforms)
  {
    for (auto& uniform : uniforms)
    {
      if (uniform.m_uniform_type == EspUniformType::ESP_BUFFER_UNIFORM)
      {
        m_buffers[uniform.m_binding] = std::move(
            std::make_unique<VulkanBuffer>(uniform.m_size_of_data_chunk,
                                           uniform.m_count_of_elements,
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

        m_buffers[uniform.m_binding]->map();
      }
    }
  }

  VulkanDescriptorSetPacgake::VulkanDescriptorSetPacgake(VulkanUniformMetaData& meta_data,
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

  void VulkanDescriptorSetPacgake::create_descriptor_set(std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
                                                         VkDescriptorPool& descriptor_pool)
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptor_pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    allocInfo.pSetLayouts        = descriptor_set_layouts.data();

    m_descriptor_sets.resize(descriptor_set_layouts.size());
    if (vkAllocateDescriptorSets(VulkanDevice::get_logical_device(), &allocInfo, m_descriptor_sets.data()) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate descriptor sets!");
      throw std::runtime_error("Failed to allocate descriptor sets!");
    }
  }

  void VulkanDescriptorSetPacgake::bind_data_and_descriptors(std::vector<VulkanMetaUniform>& uniforms,
                                                             VkDescriptorSet& ds,
                                                             DescriptorSetBuffers& ds_data)
  {
    std::vector<VkWriteDescriptorSet> descriptorWrites;
    std::vector<VkDescriptorBufferInfo> bufferInfos;

    for (auto& uniform : uniforms)
    {
      if (uniform.m_uniform_type == EspUniformType::ESP_BUFFER_UNIFORM)
      {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = ds_data.m_buffers[uniform.m_binding]->get_buffer();
        bufferInfo.offset = 0;
        bufferInfo.range  = uniform.m_size_of_data_chunk * uniform.m_count_of_elements;
        bufferInfos.push_back(bufferInfo);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet          = ds;
        descriptorWrite.dstBinding      = uniform.m_binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = uniform.m_count_of_elements;
        descriptorWrite.pBufferInfo     = &bufferInfos.back();

        descriptorWrites.push_back(descriptorWrite);
      }
      else if (uniform.m_uniform_type == EspUniformType::ESP_TEXTURE)
      {
        ESP_CORE_ERROR("NO IMPLEMENTED TEXTURES!");
        throw std::runtime_error("NO IMPLEMENTED TEXTURES!");
      }
    }

    vkUpdateDescriptorSets(VulkanDevice::get_logical_device(),
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(),
                           0,
                           nullptr);
  }

  void VulkanDescriptorSetPacgake::write(int set, int binding, int size, void* data)
  {
    m_buffer_groups[set].m_buffers[binding]->write_to_buffer(data, size);
  }

  void VulkanDescriptorSetPacgake::write(int set, int binding, int elem, int size, void* data)
  {
    m_buffer_groups[set].m_buffers[binding]->write_to_buffer(data, size, elem * size);
  }

} // namespace esp

namespace esp
{
  VulkanUniformManager::VulkanUniformManager(std::unique_ptr<VulkanUniformMetaData> meta_data) :
      m_meta_data(std::move(meta_data))
  {
    /*
      SHORTCUTS:
        dsl - Descriptor Set Layout
        dslbs - Descriptor Set Layout Bindings
    */
    m_ds_layouts = std::make_unique<std::vector<VkDescriptorSetLayout>>();

    int counter_dsl = 1;
    for (auto& meta_dsl : m_meta_data->m_meta_descriptor_sets)
    {
      std::vector<VkDescriptorSetLayoutBinding> dslbs;

      for (auto& meta_uniform : meta_dsl.m_meta_uniforms)
      {
        dslbs.push_back(create_dslb(meta_uniform));
      }

      VkDescriptorSetLayoutCreateInfo layoutInfo{};
      layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      layoutInfo.bindingCount = static_cast<uint32_t>(dslbs.size());
      layoutInfo.pBindings    = dslbs.data();

      VkDescriptorSetLayout descriptorSetLayout;
      if (vkCreateDescriptorSetLayout(VulkanDevice::get_logical_device(), &layoutInfo, nullptr, &descriptorSetLayout) !=
          VK_SUCCESS)
      {
        ESP_CORE_ERROR("Failed to create descriptor set layout!");
        throw std::runtime_error("Failed to create descriptor set layout!");
      }
      m_ds_layouts->push_back(descriptorSetLayout);

      counter_dsl += 1;
    }

    init_descriptor_pool();
    init_descriptor_sets();
  }

  VulkanUniformManager::~VulkanUniformManager()
  {
    for (auto& dsl : *m_ds_layouts)
    {
      vkDestroyDescriptorSetLayout(VulkanDevice::get_logical_device(), dsl, nullptr);
    }

    vkDestroyDescriptorPool(VulkanDevice::get_logical_device(), descriptor_pool, nullptr);
  }

  void VulkanUniformManager::init_descriptor_pool()
  {
    std::vector<VkDescriptorPoolSize> pool_sizes;

    if (m_meta_data->buffer_uniform_counter != 0)
    {
      VkDescriptorPoolSize pool_size{};
      pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      pool_size.descriptorCount =
          static_cast<uint32_t>(m_meta_data->buffer_uniform_counter * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

      pool_sizes.push_back(pool_size);
    }

    if (m_meta_data->texture_uniform_counter != 0)
    {
      VkDescriptorPoolSize pool_size{};
      pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      pool_size.descriptorCount =
          static_cast<uint32_t>(m_meta_data->texture_uniform_counter * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

      pool_sizes.push_back(pool_size);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    poolInfo.pPoolSizes    = pool_sizes.data();
    poolInfo.maxSets =
        static_cast<uint32_t>((m_meta_data->buffer_uniform_counter + m_meta_data->texture_uniform_counter) *
                              VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(VulkanDevice::get_logical_device(), &poolInfo, nullptr, &descriptor_pool) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create descriptor pool!");
      throw std::runtime_error("Failed to create descriptor pool!");
    }
  }

  void VulkanUniformManager::init_descriptor_sets()
  {
    for (int frame_idx = 0; frame_idx < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; frame_idx++)
    {
      ds_packages.emplace_back(*m_meta_data, *m_ds_layouts, descriptor_pool);
    }
  }

  void VulkanUniformManager::write(int set, int binding, int size, void* data)
  {
    ds_packages[VulkanFrameManager::get_current_frame_index()].write(set, binding, size, data);
  }

  void VulkanUniformManager::write(int set, int binding, int elem, int size, void* data)
  {
    ds_packages[VulkanFrameManager::get_current_frame_index()].write(set, binding, elem, size, data);
  }

  void VulkanUniformManager::attach_ds(const VkPipelineLayout& pipeline_layout) const
  {
    // m_descriptor_sets
    vkCmdBindDescriptorSets(VulkanFrameManager::get_current_command_buffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline_layout,
                            0,
                            ds_packages[VulkanFrameManager::get_current_frame_index()].count_ds(),
                            ds_packages[VulkanFrameManager::get_current_frame_index()].get_ds().data(),
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
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding            = data.m_binding;
    uboLayoutBinding.descriptorCount    = data.m_count_of_elements;
    uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags         = stage;
    return uboLayoutBinding;
  }

  case esp::EspUniformType::ESP_SMALL_FAST_UNIFORM:
    /* code */
    throw std::runtime_error("Not implemented ESP_SMALL_FAST_UNIFORM!");
    break;

  case esp::EspUniformType::ESP_TEXTURE:
  {
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding            = data.m_binding;
    samplerLayoutBinding.descriptorCount    = data.m_count_of_elements;
    samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags         = stage;
    return samplerLayoutBinding;
  }

  default:
    ESP_CORE_ERROR("Given uniform type doesn't exist!");
    throw std::runtime_error("Given uniform type doesn't exist!");
    break;
  }
}
