#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Uniforms/EspUniformManager.hh"

// Render API Vulkan
#include "EspUniformDataStorage.hh"
#include "Platform/Vulkan/Resources/VulkanCubemap.hh"
#include "Platform/Vulkan/Resources/VulkanTexture.hh"
#include "Platform/Vulkan/Work/VulkanSwapChain.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  using VulkanCombinedTexture = std::variant<std::shared_ptr<VulkanTexture>, std::shared_ptr<VulkanCubemap>>;

  struct EspUniformPackage
  {
   private:
    struct EspBufferSet
    {
     public:
      std::map<uint32_t, VulkanBuffer*> m_binding_to_buffer;

     public:
      inline VulkanBuffer& operator[](int binding_idx) { return *(m_binding_to_buffer[binding_idx]); }

     public:
      EspBufferSet(const EspBufferSet& other)            = delete;
      EspBufferSet& operator=(const EspBufferSet& other) = delete;

      EspBufferSet(const std::vector<EspMetaUniform>& uniforms);
      ~EspBufferSet();
    };

   private:
    std::map<uint32_t, EspBufferSet*> m_set_to_bufferset;
    std::vector<VkDescriptorSet> m_descriptor_sets;

    int m_first_descriptor_set_idx;

   private:
    void update_descriptor_set(EspBufferSet& buffer_set,
                               const VkDescriptorSet& descriptor,
                               const std::vector<EspMetaUniform>& uniforms,
                               std::map<uint32_t, std::vector<VulkanCombinedTexture>>& vec_textures);

   public:
    inline void attach(const VkPipelineLayout& pipeline_layout) const
    {
      vkCmdBindDescriptorSets(VulkanWorkOrchestrator::get_current_command_buffer(),
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipeline_layout,
                              m_first_descriptor_set_idx,
                              static_cast<uint32_t>(m_descriptor_sets.size()),
                              m_descriptor_sets.data(),
                              0,
                              nullptr);
    }

    inline EspBufferSet& operator[](int set_idx) { return *(m_set_to_bufferset[set_idx]); }

   public:
    EspUniformPackage& operator=(const EspUniformPackage& other) = delete;
    EspUniformPackage(const EspUniformPackage& other)            = delete;

    EspUniformPackage(const EspUniformDataStorage& uniform_data_storage,
                      const VkDescriptorPool& descriptor_pool,
                      std::map<uint32_t, std::map<uint32_t, std::vector<VulkanCombinedTexture>>>& textures,
                      int first_descriptor_set,
                      int last_descriptor_set);
    ~EspUniformPackage();
  };

  class VulkanUniformManager : public EspUniformManager
  {
    friend class VulkanWorker;

   private:
    // <set, <binding, vector<texture> >>
    std::map<uint32_t, std::map<uint32_t, std::vector<VulkanCombinedTexture>>> m_textures;
    std::vector<EspUniformPackage*> m_packages;

    VkDescriptorPool m_descriptor_pool;

    // These references come from this object's parent pipeline.
    const VkPipelineLayout& m_out_pipeline_layout;
    const EspUniformDataStorage& m_out_uniform_data_storage;

    int m_first_descriptor_set;

    // -1 - the whole range from m_first_descriptor_set to the end.
    int m_last_descriptor_set;

   private:
    void init_pool();

    VulkanUniformManager(const EspUniformDataStorage& uniform_data_storage,
                         const VkPipelineLayout& out_pipeline_layout,
                         int first_descriptor_set,
                         int last_descriptor_set);

   public:
    void build() override;

    inline virtual void attach() const override
    {
      m_packages[VulkanSwapChain::get_current_frame_index()]->attach(m_out_pipeline_layout);
    }

    inline virtual EspUniformManager& update_buffer_uniform(uint32_t set,
                                                            uint32_t binding,
                                                            uint64_t offset,
                                                            uint32_t size,
                                                            void* data) override
    {
      m_packages[VulkanSwapChain::get_current_frame_index()]->operator[](set)[binding].write_to_buffer(data,
                                                                                                       size,
                                                                                                       offset);
      return *this;
    }

    inline virtual EspUniformManager& load_texture(uint32_t set,
                                                   uint32_t binding,
                                                   std::shared_ptr<EspTexture> texture) override
    {
      auto vulkan_texture = std::static_pointer_cast<VulkanTexture>(texture);
      m_textures[set][binding].emplace_back(vulkan_texture);

      return *this;
    }

    inline virtual EspUniformManager& load_texture(uint32_t set,
                                                   uint32_t binding,
                                                   std::shared_ptr<EspCubemap> cubemap) override
    {
      auto vulkan_cubemap = std::static_pointer_cast<VulkanCubemap>(cubemap);
      m_textures[set][binding].emplace_back(vulkan_cubemap);

      return *this;
    }

    inline virtual EspUniformManager& update_push_uniform(uint32_t index, void* data) override
    {
      auto& push_range = m_out_uniform_data_storage.m_push_constant_ranges[index];
      vkCmdPushConstants(VulkanWorkOrchestrator::get_current_command_buffer(),
                         m_out_pipeline_layout,
                         push_range.stageFlags,
                         push_range.offset,
                         push_range.size,
                         data);

      return *this;
    }

    inline virtual EspUniformManager& load_block(uint32_t set, uint32_t binding, EspBlock* block) override
    {
      m_textures[set][binding].emplace_back(VulkanTexture::create_from_block(static_cast<VulkanBlock*>(block)));

      return *this;
    }

   public:
    ~VulkanUniformManager();
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH */
