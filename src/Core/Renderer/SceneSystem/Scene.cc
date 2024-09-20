#include "Scene.hh"
#include "Components/Components.hh"
#include "Entity.hh"

namespace esp
{
  std::shared_ptr<Scene> Scene::create() { return std::shared_ptr<Scene>(new Scene()); }

  std::shared_ptr<Entity> Scene::create_entity(const std::string& name)
  {
    Entity entity = { m_registry.create(), this };

    auto& tag = entity.add_component<TagComponent>();
    tag.m_tag = name.empty() ? "entity" : name;

    entity.add_component<TransformComponent>();
    auto& root_node = m_root_entity->get_component<NodeComponent>();
    entity.add_component<NodeComponent>(root_node.m_handle, entity.m_handle);

    return std::make_shared<Entity>(entity);
  }

  void Scene::destroy_entity(Entity& entity) { m_registry.destroy(entity.m_handle); }

  void Scene::destroy_entity(uint32_t id) { m_registry.destroy((entt::entity)id); }

  Scene::Scene() : m_root_entity{ create_root() } {}

  std::unique_ptr<Entity> Scene::create_root()
  {
    Entity entity = { m_registry.create(), this };

    auto& tag = entity.add_component<TagComponent>();
    tag.m_tag = "root";

    entity.add_component<TransformComponent>();
    entity.add_component<NodeComponent>(entity.m_handle);

    NodeComponent::s_scene = this;

    return std::make_unique<Entity>(entity);
  }
} // namespace esp