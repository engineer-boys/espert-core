#include "VulkanUniformManager.hh"

// platform
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"
#include "Platform/Vulkan/VulkanSwapChain.hh"

/* --------------------------------------------------------- */
/* ----------------- VulkanUniformManager ------------------ */
/* --------------------------------------------------------- */
namespace esp
{
  VulkanUniformManager::VulkanUniformManager(const EspUniformDataStorage& uniform_data_storage,
                                             const VkPipelineLayout& out_pipeline_layout) :
      m_out_pipeline_layout{ out_pipeline_layout },
      m_out_uniform_data_storage(uniform_data_storage)
  {
    init_pool();
  }

  VulkanUniformManager::~VulkanUniformManager()
  {
    vkDestroyDescriptorPool(VulkanDevice::get_logical_device(), m_descriptor_pool, nullptr);

    for (auto& package : m_packages)
    {
      delete package;
    }
  }

  void VulkanUniformManager::init_pool()
  {
    auto& meta_data = m_out_uniform_data_storage.m_meta_data;
    std::vector<VkDescriptorPoolSize> pool_sizes;

    if (meta_data->m_general_buffer_uniform_counter != 0)
    {
      VkDescriptorPoolSize pool_size{};
      pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      pool_size.descriptorCount =
          static_cast<uint32_t>(meta_data->m_general_buffer_uniform_counter * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

      pool_sizes.push_back(pool_size);
    }

    if (meta_data->m_general_texture_uniform_counter != 0)
    {
      VkDescriptorPoolSize pool_size{};
      pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      pool_size.descriptorCount =
          static_cast<uint32_t>(meta_data->m_general_texture_uniform_counter * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

      pool_sizes.push_back(pool_size);
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes    = pool_sizes.data();
    pool_info.maxSets       = static_cast<uint32_t>((meta_data->m_general_buffer_uniform_counter +
                                               meta_data->m_general_texture_uniform_counter +
                                               meta_data->m_general_push_uniform_counter) *
                                              VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(VulkanDevice::get_logical_device(), &pool_info, nullptr, &m_descriptor_pool) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create descriptor pool!");
      throw std::runtime_error("Failed to create descriptor pool!");
    }
  }

  void VulkanUniformManager::build()
  {
    for (int frame_idx = 0; frame_idx < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; ++frame_idx)
    {
      m_packages.push_back(new EspUniformPackage(m_out_uniform_data_storage, m_descriptor_pool, m_textures));
    }
  }
} // namespace esp

/* --------------------------------------------------------- */
/* ------------------ HELPFUL STRUCTURES ------------------- */
/* --------------------------------------------------------- */

namespace esp
{
  EspUniformPackage::EspBufferSet::EspBufferSet(const std::vector<EspMetaUniform>& uniforms)
  {
    for (auto& uniform : uniforms)
    {
      if (uniform.m_uniform_type == EspUniformType::ESP_BUFFER_UNIFORM)
      {
        ESP_ASSERT(m_binding_to_buffer.contains(uniform.m_binding) == 0, "You are trying to overwrite the biding.");

        m_binding_to_buffer[uniform.m_binding] =
            new VulkanBuffer(uniform.m_size_of_data_chunk,
                             uniform.m_number_of_elements,
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        m_binding_to_buffer[uniform.m_binding]->map();
      }
    }
  }

  EspUniformPackage::EspBufferSet::~EspBufferSet()
  {
    for (const auto& [key, val] : m_binding_to_buffer)
    {
      delete val;
    }
  }

  EspUniformPackage::EspUniformPackage(
      const EspUniformDataStorage& uniform_data_storage,
      const VkDescriptorPool& descriptor_pool,
      std::map<uint32_t, std::map<uint32_t, std::vector<std::shared_ptr<VulkanTexture>>>>& textures)
  {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool     = descriptor_pool;
    alloc_info.descriptorSetCount = uniform_data_storage.get_layouts_count();
    alloc_info.pSetLayouts        = uniform_data_storage.get_layouts_data();

    m_descriptor_sets.resize(uniform_data_storage.get_layouts_count());
    if (vkAllocateDescriptorSets(VulkanDevice::get_logical_device(), &alloc_info, m_descriptor_sets.data()) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate descriptor sets!");
      throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    for (const auto& meta_ds : uniform_data_storage.m_meta_data->m_meta_descriptor_sets)
    {
      if (meta_ds.m_buffer_uniform_counter != 0)
      {
        m_set_to_bufferset[meta_ds.m_set_index] = new EspBufferSet(meta_ds.m_meta_uniforms);
      }

      update_descriptor_set(*(m_set_to_bufferset[meta_ds.m_set_index]),
                            m_descriptor_sets[meta_ds.m_set_index],
                            meta_ds.m_meta_uniforms,
                            textures[meta_ds.m_set_index]);
    }
  }

  EspUniformPackage::~EspUniformPackage()
  {
    for (const auto& [key, val] : m_set_to_bufferset)
    {
      delete val;
    }
  }

  void EspUniformPackage::update_descriptor_set(
      EspBufferSet& buffer_set,
      const VkDescriptorSet& descriptor,
      const std::vector<EspMetaUniform>& uniforms,
      std::map<uint32_t, std::vector<std::shared_ptr<VulkanTexture>>>& vec_textures)
  {
    // these objects have to exist until updating ds
    std::vector<VkWriteDescriptorSet> descriptor_writes;
    std::vector<std::vector<VkDescriptorBufferInfo>> all_buffer_infos;
    std::vector<std::vector<VkDescriptorImageInfo>> all_image_infos;

    for (auto& uniform : uniforms)
    {
      if (uniform.m_uniform_type == EspUniformType::ESP_BUFFER_UNIFORM)
      {
        std::vector<VkDescriptorBufferInfo> buffer_infos{};

        for (int elem_idx = 0; elem_idx < uniform.m_number_of_elements; elem_idx++)
        {
          VkDescriptorBufferInfo buffer_info{};
          buffer_info.buffer = buffer_set[uniform.m_binding].get_buffer();
          buffer_info.offset = elem_idx * uniform.m_size_of_data_chunk;
          buffer_info.range  = uniform.m_size_of_data_chunk;
          buffer_infos.push_back(buffer_info);
        }
        all_buffer_infos.push_back(buffer_infos);

        VkWriteDescriptorSet descriptor_write{};
        descriptor_write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstSet          = descriptor;
        descriptor_write.dstBinding      = uniform.m_binding;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write.descriptorCount = uniform.m_number_of_elements;
        descriptor_write.pBufferInfo     = all_buffer_infos.back().data();

        descriptor_writes.push_back(descriptor_write);
      }
      else if (uniform.m_uniform_type == EspUniformType::ESP_TEXTURE)
      {
        std::vector<VkDescriptorImageInfo> image_infos{};

        for (int elem_idx = 0; elem_idx < uniform.m_number_of_elements; elem_idx++)
        {
          VulkanTexture& texture = *(vec_textures[uniform.m_binding][elem_idx]);

          VkDescriptorImageInfo image_info{};
          image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
          image_info.imageView   = texture.get_texture_image_view();
          image_info.sampler     = texture.get_sampler();
          image_infos.push_back(image_info);
        }
        all_image_infos.push_back(image_infos);

        VkWriteDescriptorSet descriptor_write{};
        descriptor_write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstSet          = descriptor;
        descriptor_write.dstBinding      = uniform.m_binding;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.descriptorCount = uniform.m_number_of_elements;
        descriptor_write.pImageInfo      = all_image_infos.back().data();

        descriptor_writes.push_back(descriptor_write);
      }
    }

    vkUpdateDescriptorSets(VulkanDevice::get_logical_device(),
                           static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(),
                           0,
                           nullptr);
  }

} // namespace esp
