#ifndef RENDERER_ESP_DESCRIPTORS_HH
#define RENDERER_ESP_DESCRIPTORS_HH

#include "EspDevice.hh"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace esp
{
  class EspDescriptorSetLayout
  {
   public:
    class Builder
    {
     private:
      EspDevice& m_device;
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};

     public:
      Builder(EspDevice& device) : m_device{ device } {}

      Builder& add_binding(uint32_t binding,
                           VkDescriptorType descriptor_type,
                           VkShaderStageFlags stage_flags,
                           uint32_t count = 1);
      std::unique_ptr<EspDescriptorSetLayout> build() const;
    };

   private:
    EspDevice& m_device;
    VkDescriptorSetLayout m_descriptor_set_layout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

   public:
    EspDescriptorSetLayout(EspDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~EspDescriptorSetLayout();

    EspDescriptorSetLayout(const EspDescriptorSetLayout&)            = delete;
    EspDescriptorSetLayout& operator=(const EspDescriptorSetLayout&) = delete;

    inline VkDescriptorSetLayout get_layout() const { return m_descriptor_set_layout; }

    friend class EspDescriptorWriter;
  };

  class EspDescriptorSet
  {
   private:
    VkDescriptorSet m_descriptor_set;

   public:
    EspDescriptorSet()  = default;
    ~EspDescriptorSet() = default;

    EspDescriptorSet(const EspDescriptorSet&)            = delete;
    EspDescriptorSet& operator=(const EspDescriptorSet&) = delete;

    // TODO: add possibility to bind group of sets
    void bind(VkCommandBuffer command_buffer, VkPipelineLayout pipeline_layout);

    inline VkDescriptorSet& get_set() { return m_descriptor_set; }
  };

  class EspDescriptorPool
  {
   public:
    class Builder
    {
     private:
      EspDevice& m_device;
      std::vector<VkDescriptorPoolSize> m_pool_sizes{};
      uint32_t m_max_sets                      = 1000;
      VkDescriptorPoolCreateFlags m_pool_flags = 0;

     public:
      Builder(EspDevice& device) : m_device{ device } {}

      Builder& add_pool_size(VkDescriptorType descriptor_type, uint32_t count);
      Builder& set_pool_flags(VkDescriptorPoolCreateFlags flags);
      Builder& set_max_sets(uint32_t count);
      std::unique_ptr<EspDescriptorPool> build() const;
    };

   private:
    EspDevice& m_device;
    VkDescriptorPool m_descriptor_pool;

   public:
    EspDescriptorPool(EspDevice& device,
                      uint32_t max_sets,
                      VkDescriptorPoolCreateFlags pool_flags,
                      const std::vector<VkDescriptorPoolSize>& pool_sizes);
    ~EspDescriptorPool();
    EspDescriptorPool(const EspDescriptorPool&)            = delete;
    EspDescriptorPool& operator=(const EspDescriptorPool&) = delete;

    bool allocate_descriptor_set(const VkDescriptorSetLayout& descriptor_set_layout, VkDescriptorSet& descriptor) const;

    void free_descriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void reset_pool();

    friend class EspDescriptorWriter;
  };

  class EspDescriptorWriter
  {
   private:
    EspDescriptorSetLayout& m_set_layout;
    EspDescriptorPool& m_pool;
    std::vector<VkWriteDescriptorSet> m_writes;

   public:
    EspDescriptorWriter(EspDescriptorSetLayout& set_layout, EspDescriptorPool& pool);

    EspDescriptorWriter& write_buffer(uint32_t binding, VkDescriptorBufferInfo* buffer_info);
    EspDescriptorWriter& write_image(uint32_t binding, VkDescriptorImageInfo* image_info);

    bool build(EspDescriptorSet& set);
    void overwrite(EspDescriptorSet& set);
  };
} // namespace esp

#endif // RENDERER_ESP_DESCRIPTORS_HH
