#include "EspDescriptors.hh"

// std
#include <stdexcept>

namespace esp
{
  // *************** Descriptor Set Layout Builder *********************

  EspDescriptorSetLayout::Builder&
  EspDescriptorSetLayout::Builder::add_binding(uint32_t binding,
                                               VkDescriptorType descriptor_type,
                                               VkShaderStageFlags stage_flags,
                                               uint32_t count)
  {
    ESP_ASSERT(m_bindings.count(binding) == 0, "Binding already in use")
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding         = binding;
    layout_binding.descriptorType  = descriptor_type;
    layout_binding.descriptorCount = count;
    layout_binding.stageFlags      = stage_flags;
    m_bindings[binding]            = layout_binding;
    return *this;
  }

  std::unique_ptr<EspDescriptorSetLayout>
  EspDescriptorSetLayout::Builder::build() const
  {
    return std::make_unique<EspDescriptorSetLayout>(m_device, m_bindings);
  }

  // *************** Descriptor Set Layout *********************

  EspDescriptorSetLayout::EspDescriptorSetLayout(
      EspDevice& device,
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings) :
      m_device{ device },
      m_bindings{ bindings }
  {
    std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
    for (auto kv : bindings)
    {
      set_layout_bindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
    descriptor_set_layout_info.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.bindingCount =
        static_cast<uint32_t>(set_layout_bindings.size());
    descriptor_set_layout_info.pBindings = set_layout_bindings.data();

    if (vkCreateDescriptorSetLayout(device.get_device(),
                                    &descriptor_set_layout_info,
                                    nullptr,
                                    &m_descriptor_set_layout) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create descriptor set layout");
      throw std::runtime_error("Failed to create descriptor set layout");
    }
  }

  EspDescriptorSetLayout::~EspDescriptorSetLayout()
  {
    vkDestroyDescriptorSetLayout(m_device.get_device(),
                                 m_descriptor_set_layout,
                                 nullptr);
  }

  // *************** Descriptor Pool Builder *********************

  EspDescriptorPool::Builder&
  EspDescriptorPool::Builder::add_pool_size(VkDescriptorType descriptor_type,
                                            uint32_t count)
  {
    m_pool_sizes.push_back({ descriptor_type, count });
    return *this;
  }

  EspDescriptorPool::Builder&
  EspDescriptorPool::Builder::set_pool_flags(VkDescriptorPoolCreateFlags flags)
  {
    m_pool_flags = flags;
    return *this;
  }
  EspDescriptorPool::Builder&
  EspDescriptorPool::Builder::set_max_sets(uint32_t count)
  {
    m_max_sets = count;
    return *this;
  }

  std::unique_ptr<EspDescriptorPool> EspDescriptorPool::Builder::build() const
  {
    return std::make_unique<EspDescriptorPool>(m_device,
                                               m_max_sets,
                                               m_pool_flags,
                                               m_pool_sizes);
  }

  // *************** Descriptor Pool *********************

  EspDescriptorPool::EspDescriptorPool(
      EspDevice& device,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags pool_flags,
      const std::vector<VkDescriptorPoolSize>& pool_sizes) :
      m_device{ device }
  {
    VkDescriptorPoolCreateInfo descriptor_pool_info{};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount =
        static_cast<uint32_t>(pool_sizes.size());
    descriptor_pool_info.pPoolSizes = pool_sizes.data();
    descriptor_pool_info.maxSets    = maxSets;
    descriptor_pool_info.flags      = pool_flags;

    if (vkCreateDescriptorPool(device.get_device(),
                               &descriptor_pool_info,
                               nullptr,
                               &m_descriptor_pool) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create descriptor pool");
      throw std::runtime_error("Failed to create descriptor pool");
    }
  }

  EspDescriptorPool::~EspDescriptorPool()
  {
    vkDestroyDescriptorPool(m_device.get_device(), m_descriptor_pool, nullptr);
  }

  bool EspDescriptorPool::allocate_descriptor_set(
      const VkDescriptorSetLayout& descriptor_set_layout,
      VkDescriptorSet& descriptor) const
  {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = m_descriptor_pool;
    alloc_info.pSetLayouts    = &descriptor_set_layout;
    alloc_info.descriptorSetCount = 1;

    // TODO: Might want to create a "DescriptorPoolManager" class that handles
    // this case,
    //  and builds a new pool whenever an old pool fills up:
    //  https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/
    if (vkAllocateDescriptorSets(m_device.get_device(),
                                 &alloc_info,
                                 &descriptor) != VK_SUCCESS)
    {
      return false;
    }
    return true;
  }

  void EspDescriptorPool::free_descriptors(
      std::vector<VkDescriptorSet>& descriptors) const
  {
    vkFreeDescriptorSets(m_device.get_device(),
                         m_descriptor_pool,
                         static_cast<uint32_t>(descriptors.size()),
                         descriptors.data());
  }

  void EspDescriptorPool::reset_pool()
  {
    vkResetDescriptorPool(m_device.get_device(), m_descriptor_pool, 0);
  }

  // *************** Descriptor Writer *********************

  EspDescriptorWriter::EspDescriptorWriter(EspDescriptorSetLayout& set_layout,
                                           EspDescriptorPool& pool) :
      m_set_layout{ set_layout },
      pool{ pool }
  {
  }

  EspDescriptorWriter&
  EspDescriptorWriter::write_buffer(uint32_t binding,
                                    VkDescriptorBufferInfo* buffer_info)
  {
    ESP_ASSERT(m_set_layout.m_bindings.count(binding) == 1,
               "Layout does not contain specified binding")

    auto& binding_description = m_set_layout.m_bindings[binding];

    ESP_ASSERT(binding_description.descriptorCount == 1,
               "Binding single descriptor info, but binding expects multiple")

    VkWriteDescriptorSet write{};
    write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType  = binding_description.descriptorType;
    write.dstBinding      = binding;
    write.pBufferInfo     = buffer_info;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
  }

  EspDescriptorWriter&
  EspDescriptorWriter::write_image(uint32_t binding,
                                   VkDescriptorImageInfo* image_info)
  {
    ESP_ASSERT(m_set_layout.m_bindings.count(binding) == 1,
               "Layout does not contain specified binding")

    auto& binding_description = m_set_layout.m_bindings[binding];

    ESP_ASSERT(binding_description.descriptorCount == 1,
               "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType  = binding_description.descriptorType;
    write.dstBinding      = binding;
    write.pImageInfo      = image_info;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
  }

  bool EspDescriptorWriter::build(VkDescriptorSet& set)
  {
    bool success =
        pool.allocate_descriptor_set(m_set_layout.get_descriptor_set_layout(),
                                     set);
    if (!success) { return false; }
    overwrite(set);
    return true;
  }

  void EspDescriptorWriter::overwrite(VkDescriptorSet& set)
  {
    for (auto& write : writes)
    {
      write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool.m_device.get_device(),
                           writes.size(),
                           writes.data(),
                           0,
                           nullptr);
  }
} // namespace esp