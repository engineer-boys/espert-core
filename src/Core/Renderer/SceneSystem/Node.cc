#include "Node.hh"
#include "Entity.hh"

using namespace esp::action;

namespace esp
{
  std::shared_ptr<Node> Node::create() { return std::shared_ptr<Node>(new Node()); }

  std::shared_ptr<Node> Node::create(std::shared_ptr<Entity> entity)
  {
    return std::shared_ptr<Node>(new Node(std::move(entity)));
  }

  std::shared_ptr<Node> Node::create_root(Scene* scene)
  {
    return std::shared_ptr<Node>(new Node(scene->create_entity()));
  }

  void Node::attach_entity(std::shared_ptr<Entity> entity) { m_entity = std::move(entity); }

  void Node::add_child(std::shared_ptr<Node> node)
  {
    node->m_parent = shared_from_this();
    m_children.emplace_back(std::move(node));
  }

  Node* Node::get_parent() { return m_parent.get(); }

  Node* Node::get_child(uint32_t index)
  {
    ESP_ASSERT(index < m_children.size(), "Index out of bounds")
    return m_children[index].get();
  }

  Entity* Node::get_entity() { return m_entity.get(); }

  void Node::translate(glm::vec3 vec)
  {
    auto& transform = get_transform();
    transform.translate(vec);
  }

  void Node::rotate(float angle, glm::vec3 axis)
  {
    auto& transform = get_transform();
    transform.rotate(angle, axis);
  }

  void Node::scale(float val)
  {
    auto& transform = get_transform();
    transform.scale(val);
  }

  void Node::set_translation(glm::vec3 vec)
  {
    auto& transform = get_transform();
    transform.set_translation(vec);
  }

  void Node::set_rotation(float angle, glm::vec3 axis)
  {
    auto& transform = get_transform();
    transform.set_rotation(angle, axis);
  }

  void Node::set_scale(float val)
  {
    auto& transform = get_transform();
    transform.set_scale(val);
  }

  glm::mat4 Node::get_model_mat(ActionType type)
  {
    auto& transform = get_transform();
    auto parent     = get_parent();

    if (type == ActionType::ESP_RELATIVE || !parent) { return transform.get_model_mat(); }
    else { return TransformComponent::calculate_model_mat(get_translation(type), get_rotation(type), get_scale(type)); }
  }

  glm::vec3 Node::get_translation(ActionType type)
  {
    auto& transform = get_transform();
    auto parent     = get_parent();

    glm::vec3 translation = transform.get_translation();
    if (type == ActionType::ESP_ABSOLUTE && parent) { return translation + parent->get_translation(type); }
    return translation;
  }

  glm::quat Node::get_rotation(ActionType type)
  {
    auto& transform = get_transform();
    auto parent     = get_parent();

    glm::quat rotation = transform.get_rotation();
    if (type == ActionType::ESP_ABSOLUTE && parent) { return parent->get_rotation(type) * rotation; }
    return rotation;
  }

  float Node::get_scale(ActionType type)
  {
    auto& transform = get_transform();
    auto parent     = get_parent();

    float scale = transform.get_scale();
    if (type == ActionType::ESP_ABSOLUTE && parent) { return scale * parent->get_scale(type); }
    return scale;
  }

  TransformComponent& Node::get_transform() { return m_entity->get_component<TransformComponent>(); }
} // namespace esp