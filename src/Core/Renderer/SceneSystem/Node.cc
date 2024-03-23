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

  void Node::add_child(Node* node)
  {
    auto found = std::find_if(m_children.begin(), m_children.end(), [&](const auto& item) { return item == node; });
    if (found != m_children.end()) { return; }

    node->set_parent(this);
    m_children.emplace_back(node);
  }

  void Node::rebase_child(esp::Node* dst, esp::Node* child)
  {
    auto found = std::find_if(m_children.begin(), m_children.end(), [&](const auto& item) { return item == child; });
    if (found == m_children.end()) { return; }

    remove_child(child);
    for (auto& c_child : child->m_children)
    {
      add_child(c_child);
      c_child->translate(child->get_translation());
    }
    if (dst) { dst->add_child(child); }
  }

  void Node::remove_child(Node* node)
  {
    auto found = std::find_if(m_children.begin(), m_children.end(), [&](const auto& item) { return item == node; });
    if (found == m_children.end()) { return; }

    (*found)->m_parent = nullptr;
    m_children.erase(found);
  }

  void Node::set_parent(esp::Node* node)
  {
    m_parent = nullptr;

    if (!m_entity->has_component<TransformComponent>()) { return; }
    set_translation(get_translation() - node->get_translation());
    // set_rotation(get_rotation()/node->get_rotation());
    set_scale(get_scale() / node->get_scale());

    m_parent = node;
  }

  Node* Node::get_parent() { return m_parent; }

  Node* Node::get_child(uint32_t index)
  {
    ESP_ASSERT(index < m_children.size(), "Index out of bounds")
    return m_children[index];
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

  void Node::set_translation(glm::vec3 vec, action::ActionType type)
  {
    auto& transform = get_transform();
    if (type == ActionType::ESP_RELATIVE)
    {
      for (auto& child : m_children)
      {
        auto& child_transform = child->get_transform();
        child_transform.translate(transform.get_translation() - vec);
      }
    }
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

    glm::mat4 model = transform.get_model_mat();
    if (type == ActionType::ESP_RELATIVE || !parent) { return model; }
    else { return parent->get_model_mat() * model; }
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