#include "NodeManager.hh"
#include "Core/Renderer/SceneSystem/Components/NodeComponent.hh"

namespace esp
{
  static Scene* s_scene;

  void NodeManager::set_scene(Scene* scene) { s_scene = scene; }

  NodeComponent& NodeManager::get_node(entt::entity id) { return s_scene->get_component<NodeComponent>(id); }

  NodeComponent& NodeManager::get_node(uint32_t id) { return s_scene->get_component<NodeComponent>(id); }
} // namespace esp