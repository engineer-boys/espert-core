#ifndef ESPERT_CORE_RESOURCES_LOADERS_BINARYLOADER_HH
#define ESPERT_CORE_RESOURCES_LOADERS_BINARYLOADER_HH

#include "esppch.hh"

#include "Core/Systems/ResourceSystem.hh"

namespace esp
{
  class BinaryLoader : public Loader
  {
   public:
    virtual std::unique_ptr<Resource>
    load(const std::string& name, ResourceType resource_type, const ResourceParams& params) override;
    virtual void unload(std::unique_ptr<Resource> resource) override;
    inline virtual const ResourceType& get_resource_type() override { return m_resource_type; }

   private:
    const ResourceType m_resource_type = typeid(BinaryResource);
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_LOADERS_BINARYLOADER_HH