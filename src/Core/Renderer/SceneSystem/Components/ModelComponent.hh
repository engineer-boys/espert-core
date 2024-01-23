#ifndef SCENE_COMPONENTS_MODEL_COMPONENT_HH
#define SCENE_COMPONENTS_MODEL_COMPONENT_HH

#include "esppch.hh"

#include "Core/Renderer/Model/Model.hh"

namespace esp
{
  /// @brief ECS component that attaches Material.
  struct ModelComponent
  {
   private:
    std::shared_ptr<Model> m_model;
    std::shared_ptr<EspShader> m_shader;
    std::unique_ptr<EspUniformManager> m_uniform_manager;

   public:
    /// @brief Default constructor.
    ModelComponent() = default;
    /// @brief Constructor that initializes model, model's shader and model's uniform manager.
    /// @param model Shared pointer to model.
    /// @param shader Shared pointer to model's shader
    /// @param start_managed_ds First managed descriptor set of model's uniform manager. 0 by default.
    /// @param end_managed_ds Last managed descriptor set of model's uniform manager. 0 by default.
    ModelComponent(std::shared_ptr<Model>& model,
                   std::shared_ptr<EspShader>& shader,
                   int start_managed_ds = 0,
                   int end_managed_ds   = 0) :
        m_model{ model },
        m_shader{ shader }, m_uniform_manager{ shader->create_uniform_manager(start_managed_ds, end_managed_ds) }
    {
      m_uniform_manager->build();
    }
    /// @brief Constructor that initializes model, model's shader and model's uniform manager.
    /// @param builder Model builder.
    /// @param start_managed_ds First managed descriptor set of model's uniform manager. 0 by default.
    /// @param end_managed_ds Last managed descriptor set of model's uniform manager. 0 by default.
    ModelComponent(Model::Builder& builder, int start_managed_ds = 0, int end_managed_ds = 0) :
        m_model{ std::make_shared<Model>(builder) }, m_shader{ builder.m_shader },
        m_uniform_manager{ builder.m_shader->create_uniform_manager(start_managed_ds, end_managed_ds) }
    {
      m_uniform_manager->build();
    }

    /// @brief Returns reference to model.
    /// @return Model reference.
    inline Model& get_model() { return *m_model; }
    /// @brief Returns reference to model shader.
    /// @return Shader reference.
    inline EspShader& get_shader() { return *m_shader; }
    /// @brief Returns reference to model uniform manager.
    /// @return Uniform manager reference.
    inline EspUniformManager& get_uniform_manager() { return *m_uniform_manager; }

    /// @brief Equals operator checks if two components reference the same model.
    /// @param other Other component to compare to.
    /// @return True if two components reference the same model. False otherwise.
    bool operator==(const ModelComponent& other) const { return m_model.get() == other.m_model.get(); }
  };

  struct NModelComponent
  {
   private:
    std::shared_ptr<NModel> m_model; // TODO: don't use ptrs
    std::shared_ptr<EspShader> m_shader;
    std::unique_ptr<EspUniformManager> m_uniform_manager;
    std::unordered_map<std::shared_ptr<Material>, std::unique_ptr<EspUniformManager>> m_material_managers;

   public:
    NModelComponent(std::shared_ptr<NModel>& model,
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
    inline NModel& get_model() { return *m_model; }
    /// @brief Returns reference to model shader.
    /// @return Shader reference.
    inline EspShader& get_shader() { return *m_shader; }
    /// @brief Returns reference to model uniform manager.
    /// @return Uniform manager reference.
    inline EspUniformManager& get_uniform_manager() { return *m_uniform_manager; }
    /// ...
    inline auto& get_material_managers() { return m_material_managers; }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_MODEL_COMPONENT_HH
