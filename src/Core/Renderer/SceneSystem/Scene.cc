#include "Scene.hh"
#include "Components/Components.hh"
#include "Entity.hh"

#include "Core/RenderAPI/Work/EspJob.hh"

// signatures
static void draw_model(const esp::ModelComponent& model_component);

/* --------------------------------------------------------- */
/* ---------------- CLASS IMPLEMENTATION ------------------- */
/* --------------------------------------------------------- */

namespace esp
{
  Camera* Scene::s_current_camera = nullptr;

  std::shared_ptr<Scene> Scene::create() { return std::shared_ptr<Scene>(new Scene()); }

  std::shared_ptr<Entity> Scene::create_entity(const std::string& name)
  {
    Entity entity = { m_registry.create(), this };

    auto& tag = entity.add_component<TagComponent>();
    tag.m_tag = name.empty() ? "entity" : name;

    entity.add_component<TransformComponent>();

    return std::make_shared<Entity>(entity);
  }

  void Scene::destroy_entity(Entity& entity) { m_registry.destroy(entity.m_handle); }

  void Scene::draw()
  {
    // TODO: optimize by
    //  - sorting shaders
    //  - grouping instances of the same model (add instancing)
    auto view = get_view<ModelComponent>();
    for (auto entity : view)
    {
      auto& model_component = view.get<ModelComponent>(entity);
      draw_model(model_component);
    }
  }
} // namespace esp

/* --------------------------------------------------------- */
/* ------------------ HELPFUL FUNCTIONS -------------------- */
/* --------------------------------------------------------- */
static void draw_model(const esp::ModelComponent& model_component)
{
  model_component.get_shader().attach();

  const auto& uniform_manager = model_component.get_uniform_manager();
  uniform_manager.attach();

  auto& model = model_component.get_model();
  for (auto model_node : model)
  {
    if (model.has_many_mesh_nodes())
    {
      uniform_manager.update_push_uniform(0, &(model_node.m_current_node->m_precomputed_transformation));
    }

    auto& material_managers = model_component.get_material_managers();
    for (auto& mesh_idx : model_node.m_current_node->m_meshes)
    {
      auto& mesh = model.m_meshes[mesh_idx];

      if (mesh.m_material) { material_managers.at(mesh.m_material)->attach(); }
      esp::EspJob::draw_indexed(mesh.m_index_count, 1, mesh.m_first_index);
    }
  }
}