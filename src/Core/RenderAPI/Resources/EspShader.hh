#ifndef RENDER_API_ESP_SHADER_HH
#define RENDER_API_ESP_SHADER_HH

#include "esppch.hh"

#include "Core/RenderAPI/Pipeline/EspPipeline.hh"
#include "Core/RenderAPI/Resources/EspShaderConfig.hh"
#include "Core/RenderAPI/Resources/EspShaderStage.hh"

namespace esp
{
  class EspShader
  {
   protected:
    EspShader(const std::string& name, std::unique_ptr<EspWorker> worker);

    std::unique_ptr<EspWorker> m_worker;
    std::string m_name;

   public:
    virtual ~EspShader() = default;
    PREVENT_COPY(EspShader);

    static std::shared_ptr<EspShader> create(const std::string& name,
                                             std::unique_ptr<SpirvResource> spirv_resource,
                                             const EspShaderConfig& config = {});
    void attach();
    std::unique_ptr<EspUniformManager> create_uniform_manager(int start_managed_ds = -1, int end_managed_ds = -1) const;
  };
} // namespace esp

#endif // RENDER_API_ESP_SHADER_HH