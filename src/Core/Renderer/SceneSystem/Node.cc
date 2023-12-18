#include "Node.hh"

namespace esp
{
  std::shared_ptr<Node> Node::create() { return std::shared_ptr<Node>(new Node()); }

  std::shared_ptr<Node> Node::create(std::shared_ptr<Entity> entity)
  {
    return std::shared_ptr<Node>(new Node(std::move(entity)));
  }

  std::unique_ptr<Node> Node::create_root() { return std::unique_ptr<Node>(new Node()); }

  void Node::attach_entity(std::shared_ptr<Entity> entity) { m_entity = std::move(entity); }

  void Node::set_parent(std::shared_ptr<Node> parent) { m_parent = std::move(parent); }

  void Node::add_child(std::shared_ptr<Node> child) { m_children.emplace_back(std::move(child)); }

  Node* Node::get_parent() { return m_parent.get(); }

  Entity* Node::get_entity() { return m_entity.get(); }
} // namespace esp