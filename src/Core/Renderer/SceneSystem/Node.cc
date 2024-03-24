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
    for (auto& grand_child : child->m_children)
    {
      child->remove_child(grand_child);
      add_child(grand_child);
    }
    if (dst) { dst->add_child(child); }
  }

  void Node::remove_child(Node* node)
  {
    auto found = std::find_if(m_children.begin(), m_children.end(), [&](const auto& item) { return item == node; });
    if (found == m_children.end()) { return; }

    glm::mat4 model_mat = node->get_model_mat();
    (*found)->set_translation(glm::vec3(model_mat[3]));
    float scale_x          = glm::length(glm::vec3(model_mat[0]));
    float scale_y          = glm::length(glm::vec3(model_mat[1]));
    float scale_z          = glm::length(glm::vec3(model_mat[2]));
    glm::mat4 rotation_mat = glm::mat3(model_mat[0] / scale_x, model_mat[1] / scale_y, model_mat[2] / scale_z);
    (*found)->set_rotation(glm::quat_cast(rotation_mat));
    (*found)->set_scale(scale_x);

    (*found)->m_parent = nullptr;
    m_children.erase(found);
  }

  void Node::set_parent(esp::Node* node)
  {
    glm::mat4 inv_model = glm::inverse(node->get_model_mat());
    translate(glm::vec3(inv_model[3]));
    float scale_x          = glm::length(glm::vec3(inv_model[0]));
    float scale_y          = glm::length(glm::vec3(inv_model[1]));
    float scale_z          = glm::length(glm::vec3(inv_model[2]));
    glm::mat4 rotation_mat = glm::mat3(inv_model[0] / scale_x, inv_model[1] / scale_y, inv_model[2] / scale_z);
    rotate(glm::quat_cast(rotation_mat));
    scale(glm::length(glm::vec3(inv_model[0])));

    m_parent = nullptr;
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

  void Node::rotate(glm::quat quat)
  {
    auto& transform = get_transform();
    transform.rotate(quat);
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

  void Node::set_rotation(glm::quat quat)
  {
    auto& transform = get_transform();
    transform.set_rotation(quat);
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