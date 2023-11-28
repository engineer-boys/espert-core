#include "SceneNode.hh"

namespace esp
{
  std::shared_ptr<SceneNode> SceneNode::create() { return std::shared_ptr<SceneNode>(new SceneNode()); }

  std::shared_ptr<SceneNode> SceneNode::create(std::shared_ptr<Entity> entity)
  {
    return std::shared_ptr<SceneNode>(new SceneNode(std::move(entity)));
  }

  std::unique_ptr<SceneNode> SceneNode::create_root() { return std::unique_ptr<SceneNode>(new SceneNode()); }

  void SceneNode::attach_entity(std::shared_ptr<Entity> entity) { m_entity = std::move(entity); }

  void SceneNode::set_parent(std::shared_ptr<SceneNode> parent) { m_parent = std::move(parent); }

  void SceneNode::add_child(std::shared_ptr<SceneNode> child) { m_children.emplace_back(std::move(child)); }

  SceneNode* SceneNode::get_parent() { return m_parent.get(); }

  Entity* SceneNode::get_entity() { return m_entity.get(); }
} // namespace esp