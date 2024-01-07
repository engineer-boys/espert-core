#include "Action.hh"
#include "Entity.hh"
#include "Node.hh"

namespace esp::action
{
  Action<void(Node*)> TransformAction::reset = [](Node* node)
  {
    auto& transform = get_transform(node);
    transform.reset();
  };

  Action<void(Node*, glm::vec3, ActionType)> TransformAction::set_translation =
      [](Node* node, glm::vec3 translation, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ESP_ABSOLUTE || !parent) { transform.m_translation = translation; }
    else { transform.m_translation = translation + get_transform(parent).m_translation; }

    transform.translate();
  };

  Action<void(Node*, glm::vec3, ActionType)> TransformAction::update_translation =
      [](Node* node, glm::vec3 translation, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ESP_ABSOLUTE || !parent) { transform.m_translation += translation; }
    else { transform.m_translation += translation + get_transform(parent).m_translation; }

    transform.translate();
  };

  Action<void(Node*, ActionType)> TransformAction::translate = [](Node* node, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    transform.translate();
    if (type == ActionType::ESP_RELATIVE && parent) { translate(parent, type); }
  };

  Action<void(Node*, float, ActionType)> TransformAction::set_scale = [](Node* node, float s, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ESP_ABSOLUTE || !parent) { transform.m_scale = s; }
    else { transform.m_scale = s * get_transform(parent).m_scale; }

    transform.scale();
  };

  Action<void(Node*, float, ActionType)> TransformAction::update_scale = [](Node* node, float s, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ESP_ABSOLUTE || !parent) { transform.m_scale *= s; }
    else { transform.m_scale *= s * get_transform(parent).m_scale; }

    transform.scale();
  };

  Action<void(Node*, ActionType)> TransformAction::scale = [](Node* node, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    transform.scale();
    if (type == ActionType::ESP_RELATIVE && parent) { scale(parent, type); }
  };

  Action<void(Node*, float, glm::vec3, ActionType)> TransformAction::set_rotation =
      [](Node* node, float angle, glm::vec3 axis, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ESP_ABSOLUTE || !parent) { transform.m_rotation = glm::angleAxis(angle, esp::normalize(axis)); }
    else { transform.m_rotation = glm::angleAxis(angle, esp::normalize(axis)) * get_transform(parent).m_rotation; }

    transform.rotate();
  };

  Action<void(Node*, float, glm::vec3, ActionType)> TransformAction::update_rotation =
      [](Node* node, float angle, glm::vec3 axis, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ESP_ABSOLUTE || !parent)
    {
      transform.m_rotation *= glm::angleAxis(angle, esp::normalize(axis));
    }
    else { transform.m_rotation *= glm::angleAxis(angle, esp::normalize(axis)) * get_transform(parent).m_rotation; }

    transform.rotate();
  };

  Action<void(Node*, ActionType)> TransformAction::rotate = [](Node* node, ActionType type)
  {
    auto& transform = get_transform(node);
    auto parent     = node->get_parent();

    transform.rotate();
    if (type == ActionType::ESP_RELATIVE && parent) { rotate(parent, type); }
  };

  TransformComponent& TransformAction::get_transform(Node* node)
  {
    return node->get_entity()->get_component<TransformComponent>();
  }

} // namespace esp::action