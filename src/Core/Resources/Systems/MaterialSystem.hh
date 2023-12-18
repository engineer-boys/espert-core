#ifndef ESPERT_CORE_SYSTEMS_MATERIAL_SYSTEM_HH
#define ESPERT_CORE_SYSTEMS_MATERIAL_SYSTEM_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Pipeline/EspPipeline.hh"
#include "Core/RenderAPI/Resources/EspTexture.hh"

namespace esp
{
  class Material
  {
   private:
    std::unique_ptr<EspUniformManager> m_material_manager;

   public:
    static std::shared_ptr<Material> create(EspWorker& pipeline, std::vector<std::shared_ptr<EspTexture>>& textures);

    void attach();

   private:
    Material(EspWorker& pipeline, std::vector<std::shared_ptr<EspTexture>>& textures);
  };
} // namespace esp

#endif // ESPERT_CORE_SYSTEMS_MATERIAL_SYSTEM_HH
