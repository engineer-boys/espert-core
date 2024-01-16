#include "Scene.hh"
#include "Components/Components.hh"
#include "Entity.hh"

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
} // namespace esp