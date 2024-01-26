#ifndef SCENE_COMPONENTS_MODEL_COMPONENT_HH
#define SCENE_COMPONENTS_MODEL_COMPONENT_HH

#include "esppch.hh"

#include "Core/Renderer/Model/Model.hh"

namespace esp
{
  /// @brief ECS component that holds necessary classes for drawing a model. Used by Scene (TODO: ... Renderer)
  struct ModelComponent
  {
   private:
    std::shared_ptr<Model> m_model; // TODO: don't use ptrs
    std::shared_ptr<EspShader> m_shader;
    std::unique_ptr<EspUniformManager> m_uniform_manager;
    std::unordered_map<std::shared_ptr<Material>, std::unique_ptr<EspUniformManager>> m_material_managers;

   public:
    /// @brief Constructor for ModelComponent.
    /// @param model Shared pointer to model.
    /// @param shader Shared pointer to model's shader
    /// @param start_managed_ds_for_uniform_manager First descriptor set of model's uniform manager. 0 by default.
    /// @param end_managed_ds_for_uniform_manager Last descriptor set of model's uniform manager. 0 by default.
    /// @param managed_ds_for_material_manager Descriptor set of model's material managers. 1 by default.
    ModelComponent(std::shared_ptr<Model>& model,
                   std::shared_ptr<EspShader>& shader,
                   int start_managed_ds_for_uniform_manager = 0,
                   int end_managed_ds_for_uniform_manager   = 0,
                   int managed_ds_for_material_manager      = 1) :
        m_model{ model },
        m_shader{ shader }
    {
      m_uniform_manager =
          m_shader->create_uniform_manager(start_managed_ds_for_uniform_manager, end_managed_ds_for_uniform_manager);
      m_uniform_manager->build();

      for (auto& mesh : m_model->m_meshes)
      {
        if (mesh.m_material && !m_material_managers.contains(mesh.m_material))
        {
          m_material_managers.insert({ mesh.m_material, mesh.m_material->create_uniform_manager(m_shader) });
        }
      }
    }

    /// @brief Returns reference to model.
    /// @return Model reference.
    inline Model& get_model() const { return *m_model; }
    /// @brief Returns reference to model shader.
    /// @return Shader reference.
    inline EspShader& get_shader() const { return *m_shader; }
    /// @brief Returns reference to model uniform manager.
    /// @return Uniform manager reference.
    inline EspUniformManager& get_uniform_manager() const { return *m_uniform_manager; }
    /// @brief Returns reference to model material managers.
    /// @return Material managers reference.
    inline auto& get_material_managers() const { return m_material_managers; }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_MODEL_COMPONENT_HH
