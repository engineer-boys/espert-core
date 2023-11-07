#ifndef CORE_RENDER_API_ESP_UNIFORM_MANAGER_HH
#define CORE_RENDER_API_ESP_UNIFORM_MANAGER_HH

#include "esppch.hh"

namespace esp
{
  struct EspUniformManager
  {
   public:
    virtual ~EspUniformManager() {}

    virtual void build() = 0;

    virtual void attach() const = 0;

    virtual EspUniformManager&
    update_buffer_uniform(uint32_t set, uint32_t binding, uint64_t offset, uint32_t size, void* data) = 0;

    virtual EspUniformManager&
    load_texture(uint32_t set, uint32_t binding, std::string path_to_texture, bool mipmapping = false) = 0;
  };

} // namespace esp

#endif /* CORE_RENDER_API_ESP_UNIFORM_MANAGER_HH */