#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Uniforms/EspUniformManager.hh"

// platform
#include "EspUniformDataStorage.hh"
#include "Platform/Vulkan/Resources/VulkanTexture.hh"
#include "Platform/Vulkan/VulkanFrameManager.hh"

namespace esp
{
  struct EspUniformPackage
  {
   public:
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

   public:
    std::map<uint32_t, EspBufferSet*> m_set_to_bufferset;
    std::vector<VkDescriptorSet> m_descriptor_sets;

   private:
    void update_descriptor_set(EspBufferSet& buffer_set,
                               const VkDescriptorSet& descriptor,
                               const std::vector<EspMetaUniform>& uniforms,
                               std::map<uint32_t, std::vector<std::unique_ptr<VulkanTexture>>>& vec_textures);

   public:
    inline void attach(const VkPipelineLayout& pipeline_layout) const
    {
      vkCmdBindDescriptorSets(VulkanFrameManager::get_current_command_buffer(),
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipeline_layout,
                              0,
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
                      std::map<uint32_t, std::map<uint32_t, std::vector<std::unique_ptr<VulkanTexture>>>>& textures);
    ~EspUniformPackage();
  };

  class VulkanUniformManager : public EspUniformManager
  {
    friend class VulkanPipeline;

   private:
    std::map<uint32_t, std::map<uint32_t, std::vector<std::unique_ptr<VulkanTexture>>>> m_textures;
    std::vector<EspUniformPackage*> m_packages;

    VkDescriptorPool m_descriptor_pool;

    // These references come from this object's parent pipeline.
    const VkPipelineLayout& m_out_pipeline_layout;
    const EspUniformDataStorage& m_out_uniform_data_storage;

   private:
    void init_pool();

    VulkanUniformManager(const EspUniformDataStorage& uniform_data_storage,
                         const VkPipelineLayout& out_pipeline_layout);

   public:
    void build() override;

    inline virtual void attach() const override
    {
      m_packages[VulkanFrameManager::get_current_frame_index()]->attach(m_out_pipeline_layout);
    }

    inline virtual EspUniformManager&
    update_buffer_uniform(uint32_t set, uint32_t binding, uint64_t offset, uint32_t size, void* data) override
    {
      m_packages[VulkanFrameManager::get_current_frame_index()]->operator[](set)[binding].write_to_buffer(data,
                                                                                                          size,
                                                                                                          offset);
      return *this;
    }

    inline virtual EspUniformManager& load_texture(uint32_t set, uint32_t binding, std::string path_to_texture) override
    {
      m_textures[set][binding].emplace_back(VulkanTexture::create(path_to_texture));

      return *this;
    }

   public:
    ~VulkanUniformManager();
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_MANAGER_HH */
