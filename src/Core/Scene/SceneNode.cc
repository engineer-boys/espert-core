#include "SceneNode.hh"

namespace esp
{
  std::shared_ptr<SceneNode> SceneNode::create() { return std::shared_ptr<SceneNode>(new SceneNode()); }

  std::shared_ptr<SceneNode> SceneNode::create(const std::shared_ptr<Entity>& entity)
  {
    return std::shared_ptr<SceneNode>(new SceneNode(entity));
  }

  std::unique_ptr<SceneNode> SceneNode::create_root() { return std::unique_ptr<SceneNode>(new SceneNode()); }

  void SceneNode::attach_entity(const std::shared_ptr<Entity>& entity) { m_entity = entity; }

  void SceneNode::add_child(const std::shared_ptr<SceneNode>& child) { m_children.emplace_back(child); }
} // namespace esp