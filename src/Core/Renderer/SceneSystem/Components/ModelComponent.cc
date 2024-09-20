#include "ModelComponent.hh"

namespace esp
{
  ModelDrawData::ModelDrawData(const ModelComponentParams& params) :
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

  ModelComponent::ModelComponent(std::shared_ptr<Model>& model,
                                 std::shared_ptr<EspShader>& shader,
                                 int start_managed_ds_for_uniform_manager,
                                 int end_managed_ds_for_uniform_manager)
  {
    m_models_count = 1;
    m_draw_data[0] = { { model, shader, start_managed_ds_for_uniform_manager, end_managed_ds_for_uniform_manager } };
  }

  ModelComponent::ModelComponent(std::vector<ModelComponentParams> params)
  {
    ESP_ASSERT(params.size() < MAX_MODELS, "Model component can have at most 3 models")

    m_models_count = params.size();
    for (auto i = 0; i < m_models_count; i++)
    {
      m_draw_data[i] = { params[i] };
    }
  }

  Model& ModelComponent::get_model(uint32_t idx) const { return *(m_draw_data[idx].m_model); }
  EspShader& ModelComponent::get_shader(uint32_t idx) const { return *(m_draw_data[idx].m_shader); }
  EspUniformManager& ModelComponent::get_uniform_manager(uint32_t idx) const
  {
    return *(m_draw_data[idx].m_uniform_manager);
  }
  auto& ModelComponent::get_material_manager(uint32_t idx) const { return m_draw_data[idx].m_material_managers; }

  void ModelComponent::choose(uint32_t idx) { m_draw_data[idx].m_draw = true; }

  void ModelComponent::skip(uint32_t idx) { m_draw_data[idx].m_draw = false; }

  void ModelComponent::update_buffer_uniform(uint32_t set, uint32_t binding, uint64_t offset, uint32_t size, void* data)
  {
    for (auto i = 0; i < m_models_count; i++)
    {
      m_draw_data[i].m_uniform_manager->update_buffer_uniform(set, binding, offset, size, data);
    }
  }

  void esp::ModelComponent::draw() const
  {
    for (auto i = 0; i < m_models_count; i++)
    {
      auto& draw = m_draw_data[i];
      if (!draw.m_draw) { return; }

      draw.m_shader->attach();

      const auto& uniform_manager = *draw.m_uniform_manager;
      uniform_manager.attach();

      auto& model = *draw.m_model;
      for (auto model_node : model)
      {
        if (model.has_many_mesh_nodes())
        {
          uniform_manager.update_push_uniform(0, &(model_node.m_current_node->m_precomputed_transformation));
        }

        auto& material_managers = draw.m_material_managers;
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