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

  /// @brief ECS component that holds necessary classes for drawing a model. Used by Scene (TODO: ... Renderer)
  struct ModelComponent
  {
   private:
    struct ModelDrawData
    {
      std::shared_ptr<Model> m_model;
      std::shared_ptr<EspShader> m_shader;
      std::shared_ptr<EspUniformManager> m_uniform_manager;
      std::unordered_map<std::shared_ptr<Material>, std::unique_ptr<EspUniformManager>> m_material_managers;

      ModelDrawData(const ModelComponentParams& params) : m_model{ params.m_model }, m_shader{ params.m_shader }
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
    };

    std::vector<std::shared_ptr<ModelDrawData>> m_draw_data{};
    std::vector<uint32_t> m_model_indexes{};
    std::vector<EspUniformManager*> m_uniform_managers{};

   public:
    /// @brief Constructor for ModelComponent.
    /// @param model Shared pointer to model.
    /// @param shader Shared pointer to model's shader
    /// @param start_managed_ds_for_uniform_manager First descriptor set of model's uniform manager. 0 by default.
    /// @param end_managed_ds_for_uniform_manager Last descriptor set of model's uniform manager. 0 by default.
    ModelComponent(std::shared_ptr<Model>& model,
                   std::shared_ptr<EspShader>& shader,
                   int start_managed_ds_for_uniform_manager = 0,
                   int end_managed_ds_for_uniform_manager   = 0)
    {
      m_draw_data.push_back(std::make_shared<ModelDrawData>(ModelComponentParams(model,
                                                                                 shader,
                                                                                 start_managed_ds_for_uniform_manager,
                                                                                 end_managed_ds_for_uniform_manager)));

      prepare_uniform_managers();
      choose_all();
    }

    ModelComponent(std::vector<ModelComponentParams> params)
    {
      for (auto& param : params)
      {
        m_draw_data.push_back(std::make_unique<ModelDrawData>(param));
      }

      prepare_uniform_managers();
      choose_all();
    }

    /// @brief Returns reference to model.
    /// @return Model reference.
    inline Model& get_model(uint32_t idx = 0) const { return *(m_draw_data[idx]->m_model); }
    /// @brief Returns reference to model shader.
    /// @return Shader reference.
    inline EspShader& get_shader(uint32_t idx = 0) const { return *(m_draw_data[idx]->m_shader); }
    /// @brief Returns reference to model uniform manager.
    /// @return Uniform manager reference.
    inline EspUniformManager& get_uniform_manager(uint32_t idx = 0) const
    {
      return *(m_draw_data[idx]->m_uniform_manager);
    }
    inline std::vector<EspUniformManager*> get_uniform_managers() const { return m_uniform_managers; }
    /// @brief Returns reference to model material managers.
    /// @return Material managers reference.
    inline auto& get_material_managers(uint32_t idx = 0) const { return m_draw_data[idx]->m_material_managers; }

    inline void choose_all()
    {
      m_model_indexes.resize(m_draw_data.size());
      std::iota(m_model_indexes.begin(), m_model_indexes.end(), 0);
    }

    inline void choose(std::vector<uint32_t> model_indexes = {}) { m_model_indexes = std::move(model_indexes); }

    inline void draw()
    {
      for (auto i : m_model_indexes)
      {
        get_shader(i).attach();

        const auto& uniform_manager = get_uniform_manager(i);
        uniform_manager.attach();

        auto& model = get_model(i);
        for (auto model_node : model)
        {
          if (model.has_many_mesh_nodes())
          {
            uniform_manager.update_push_uniform(0, &(model_node.m_current_node->m_precomputed_transformation));
          }

          auto& material_managers = get_material_managers(i);
          for (auto& mesh_idx : model_node.m_current_node->m_meshes)
          {
            auto& mesh = model.m_meshes[mesh_idx];

            if (mesh.m_material) { material_managers.at(mesh.m_material)->attach(); }
            EspJob::draw_indexed(mesh.m_index_count, 1, mesh.m_first_index);
          }
        }
      }
    }

   private:
    inline void prepare_uniform_managers()
    {
      std::transform(m_draw_data.begin(),
                     m_draw_data.end(),
                     std::back_inserter(m_uniform_managers),
                     [](std::shared_ptr<ModelDrawData> data) { return data->m_uniform_manager.get(); });
    }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_MODEL_COMPONENT_HH
