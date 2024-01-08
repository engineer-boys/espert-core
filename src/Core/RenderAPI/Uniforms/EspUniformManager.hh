#ifndef CORE_RENDER_API_ESP_UNIFORM_MANAGER_HH
#define CORE_RENDER_API_ESP_UNIFORM_MANAGER_HH

#include "esppch.hh"

#include "Core/RenderAPI/PipelineOrdering/Block/EspBlock.hh"
#include "Core/RenderAPI/PipelineOrdering/EspCommandBuffer.hh"
#include "Core/Resources/Systems/TextureSystem.hh"
#include "EspUniformMetaData.hh"

namespace esp
{
  struct EspUniformManager
  {
   public:
    virtual ~EspUniformManager() {}

    virtual void build() = 0;

    virtual void attach() const                       = 0;
    virtual void attach(EspCommandBufferId* id) const = 0;

    virtual EspUniformManager& update_buffer_uniform(uint32_t set,
                                                     uint32_t binding,
                                                     uint64_t offset,
                                                     uint32_t size,
                                                     void* data) = 0;

    virtual EspUniformManager& set_buffer_uniform(uint32_t set,
                                                  uint32_t binding,
                                                  uint64_t offset,
                                                  uint32_t size,
                                                  void* data) = 0;

    virtual EspUniformManager& load_block(uint32_t set, uint32_t binding, EspBlock* block) = 0;

    virtual EspUniformManager& load_texture(uint32_t set, uint32_t binding, std::shared_ptr<EspTexture> texture) = 0;

    virtual EspUniformManager& update_push_uniform(uint32_t index, void* data) = 0;
    virtual EspUniformManager& update_push_uniform(EspCommandBufferId* id, uint32_t index, void* data) = 0;
  };

} // namespace esp

#endif /* CORE_RENDER_API_ESP_UNIFORM_MANAGER_HH */