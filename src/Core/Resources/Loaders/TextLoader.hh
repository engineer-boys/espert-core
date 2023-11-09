#ifndef ESPERT_CORE_RESOURCES_LOADERS_TEXTLOADER_HH
#define ESPERT_CORE_RESOURCES_LOADERS_TEXTLOADER_HH

#include "esppch.hh"

#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  class TextLoader : public Loader
  {
   public:
    virtual std::unique_ptr<Resource> load(const fs::path& path, const ResourceParams& params) override;
    virtual void unload(std::unique_ptr<Resource> resource) override;
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_LOADERS_TEXTLOADER_HH