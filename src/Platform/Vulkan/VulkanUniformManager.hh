#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH

#include "esppch.hh"

// std
#include <map>

// libs
#include "volk.h"

// Platform
#include "Buffers/VulkanBuffer.hh"
#include "VulkanSwapChain.hh"
#include "VulkanUniformMetaData.hh"

namespace esp
{
  struct VulkanDescriptorSetPackage
  {
   public:
    struct DescriptorSetBuffers
    {
      std::map<int, std::unique_ptr<VulkanBuffer>> m_buffers;

      DescriptorSetBuffers(std::vector<VulkanMetaUniform>& uniforms);
    };

   private:
    void create_descriptor_set(std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
                               VkDescriptorPool& descriptor_pool);
    void bind_data_and_descriptors(std::vector<VulkanMetaUniform>& uniforms,
                                   VkDescriptorSet& ds,
                                   DescriptorSetBuffers& ds_data);

    std::vector<VkDescriptorSet> m_descriptor_sets;
    std::vector<DescriptorSetBuffers> m_buffer_groups;

   public:
    VulkanDescriptorSetPackage(VulkanUniformMetaData& meta_data,
                               std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
                               VkDescriptorPool& descriptor_pool);

    void write(int set, int binding, int size, void* data);
    void write(int set, int binding, int elem, int size, void* data);

    inline const std::vector<VkDescriptorSet>& get_ds() const { return m_descriptor_sets; }
    inline uint32_t count_ds() const { return static_cast<uint32_t>(m_descriptor_sets.size()); }
  };

  struct VulkanManagerTexture
  {
  };

  class VulkanUniformManager
  {
   private:
    std::vector<VkDescriptorSetLayout> m_ds_layouts;
    std::unique_ptr<VulkanUniformMetaData> m_meta_data;

    VkDescriptorPool m_descriptor_pool;
    std::vector<VulkanDescriptorSetPackage> m_descriptor_set_packages;

   private:
    void init_descriptor_pool();
    void init_descriptor_sets();

   public:
    VulkanUniformManager(std::unique_ptr<VulkanUniformMetaData> meta_data);

    ~VulkanUniformManager();

    inline std::vector<VkDescriptorSetLayout>& get_layouts() { return m_ds_layouts; }
    inline uint32_t get_layouts_number() { return static_cast<uint32_t>(m_ds_layouts.size()); }

    void write(int set, int binding, int size, void* data);
    void write(int set, int binding, int elem, int size, void* data);

    void attach_descriptor_set_package(const VkPipelineLayout& pipeline_layout) const;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH */
