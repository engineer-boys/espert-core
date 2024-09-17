#ifndef SCENE_COMPONENTS_MODEL_COMPONENT_HH
#define SCENE_COMPONENTS_MODEL_COMPONENT_HH

#include "esppch.hh"

#include "Core/RenderAPI/Work/EspJob.hh"
#include "Core/Renderer/Model/Model.hh"

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
    std::unordered_map<std::shared_ptr<Material>, std::unique_ptr<EspUniformManager>> m_material_managers;
    bool m_draw{ true };

    ModelDrawData(const ModelComponentParams& params);
  };

  struct ModelComponent
  {
   private:
    std::vector<std::shared_ptr<ModelDrawData>> m_draw_data{};

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
    auto& get_draw_data() const;

    inline void choose(uint32_t idx) { m_draw_data[idx]->m_draw = true; }
    inline void skip(uint32_t idx) { m_draw_data[idx]->m_draw = false; }
    // inline ModelDrawData& operator[](uint32_t idx) { return *(m_draw_data[idx]); }

    void draw() const;
  };

  inline ModelDrawData::ModelDrawData(const ModelComponentParams& params) :
      m_model{ params.m_model }, m_shader{ params.m_shader }
  {
    m_uniform_manager = m_shader->create_uniform_manager(params.m_start_managed_ds_for_uniform_manager,
                                                         params.m_end_managed_ds_for_uniform_manager);
    m_uniform_manager->build();

    for (auto& mesh : m_model->m_meshes)
    {
      if (mesh.m_material && !m_material_managers.contains(mesh.m_material))
      {
        m_material_managers.insert({ mesh.m_material, mesh.m_material->create_uniform_manager(m_shader) });
      }
    }
  }

  inline ModelComponent::ModelComponent(std::shared_ptr<Model>& model,
                                        std::shared_ptr<EspShader>& shader,
                                        int start_managed_ds_for_uniform_manager,
                                        int end_managed_ds_for_uniform_manager)
  {
    m_draw_data.push_back(std::make_shared<ModelDrawData>(
        ModelComponentParams(model, shader, start_managed_ds_for_uniform_manager, end_managed_ds_for_uniform_manager)));
  }

  inline ModelComponent::ModelComponent(std::vector<ModelComponentParams> params)
  {
    for (auto& param : params)
    {
      m_draw_data.push_back(std::make_unique<ModelDrawData>(param));
    }
  }

  inline Model& ModelComponent::get_model(uint32_t idx) const { return *(m_draw_data[idx]->m_model); }
  inline EspShader& ModelComponent::get_shader(uint32_t idx) const { return *(m_draw_data[idx]->m_shader); }
  inline EspUniformManager& ModelComponent::get_uniform_manager(uint32_t idx) const
  {
    return *(m_draw_data[idx]->m_uniform_manager);
  }
  inline auto& ModelComponent::get_material_manager(uint32_t idx) const
  {
    return m_draw_data[idx]->m_material_managers;
  }
  inline auto& esp::ModelComponent::get_draw_data() const { return m_draw_data; }

  inline void esp::ModelComponent::draw() const
  {
    for (const auto& draw : m_draw_data)
    {
      if (!draw->m_draw) { return; }

      draw->m_shader->attach();

      const auto& uniform_manager = *draw->m_uniform_manager;
      uniform_manager.attach();

      auto& model = *draw->m_model;
      for (auto model_node : model)
      {
        if (model.has_many_mesh_nodes())
        {
          uniform_manager.update_push_uniform(0, &(model_node.m_current_node->m_precomputed_transformation));
        }

        auto& material_managers = draw->m_material_managers;
        for (auto& mesh_idx : model_node.m_current_node->m_meshes)
        {
          auto& mesh = model.m_meshes[mesh_idx];

          if (mesh.m_material) { material_managers.at(mesh.m_material)->attach(); }
          EspJob::draw_indexed(mesh.m_index_count, 1, mesh.m_first_index);
        }
      }
    }
  }
} // namespace esp

#endif // SCENE_COMPONENTS_MODEL_COMPONENT_HH
