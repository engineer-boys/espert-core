#ifndef SCENE_COMPONENTS_MODEL_COMPONENT_HH
#define SCENE_COMPONENTS_MODEL_COMPONENT_HH

#include "esppch.hh"

#include "Core/RenderAPI/Work/EspJob.hh"
#include "Core/Renderer/Model/Model.hh"

#define MAX_MODELS 3

namespace esp
{
  struct ModelComponentParams
  {
    std::shared_ptr<Model> m_model;
    std::shared_ptr<EspShader> m_shader;
    int m_start_managed_ds_for_uniform_manager = 0;
    int m_end_managed_ds_for_uniform_manager   = 0;
  };

  struct ModelDrawData
  {
    std::shared_ptr<Model> m_model;
    std::shared_ptr<EspShader> m_shader;
    std::shared_ptr<EspUniformManager> m_uniform_manager;
    std::unordered_map<std::shared_ptr<Material>, std::shared_ptr<EspUniformManager>> m_material_managers;
    bool m_draw{ true };

    ModelDrawData() = default;
    ModelDrawData(const ModelComponentParams& params);
  };

  struct ModelComponent
  {
   private:
    std::array<ModelDrawData, MAX_MODELS> m_draw_data;
    uint32_t m_models_count;

   public:
    ModelComponent(std::shared_ptr<Model>& model,
                   std::shared_ptr<EspShader>& shader,
                   int start_managed_ds_for_uniform_manager = 0,
                   int end_managed_ds_for_uniform_manager   = 0);

    ModelComponent(std::vector<ModelComponentParams> params);

    Model& get_model(uint32_t idx = 0) const;
    EspShader& get_shader(uint32_t idx = 0) const;
    EspUniformManager& get_uniform_manager(uint32_t idx = 0) const;
    auto& get_material_manager(uint32_t idx = 0) const;

    void choose(uint32_t idx);
    void skip(uint32_t idx);
    void update_buffer_uniform(uint32_t set,
                               uint32_t binding,
                               uint64_t offset,
                               uint32_t size,
                               void* data); // for each draw data
    void draw() const;
  };
} // namespace esp

#endif // SCENE_COMPONENTS_MODEL_COMPONENT_HH
