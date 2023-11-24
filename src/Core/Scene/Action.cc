#include "Action.hh"
#include "Entity.hh"
#include "SceneNode.hh"

namespace esp::action
{
  Action<void(SceneNode*, glm::vec3, ActionType)> TransformAction::set_translation =
      [](SceneNode* node, glm::vec3 translation, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent) { transform.m_translation = translation; }
    else { transform.m_translation = translation + get_translation(parent, type); }

    transform.translate();
  };

  Action<void(SceneNode*, glm::vec3, ActionType)> TransformAction::update_translation =
      [](SceneNode* node, glm::vec3 translation, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent) { transform.m_translation += translation; }
    else { transform.m_translation += translation + get_translation(parent, type); }

    transform.translate();
  };

  Action<glm::vec3(SceneNode*, ActionType)> TransformAction::get_translation = [](SceneNode* node, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent) { return transform.m_translation; }

    return transform.m_translation + get_translation(parent, type);
  };

  Action<void(SceneNode*, ActionType)> TransformAction::translate = [](SceneNode* node, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    transform.translate();
    if (type == ActionType::RELATIVE && parent) { translate(parent, type); }
  };

  Action<void(SceneNode*, float, ActionType)> TransformAction::set_scale = [](SceneNode* node, float s, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent) { transform.m_scale = s; }
    else { transform.m_scale = s * get_scale(parent, type); }

    transform.scale();
  };

  Action<void(SceneNode*, float, ActionType)> TransformAction::update_scale =
      [](SceneNode* node, float s, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent) { transform.m_scale *= s; }
    else { transform.m_scale *= s * get_scale(parent, type); }

    transform.scale();
  };

  Action<float(SceneNode*, ActionType)> TransformAction::get_scale = [](SceneNode* node, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent) { return transform.m_scale; }

    return transform.m_scale + get_scale(parent, type);
  };

  Action<void(SceneNode*, ActionType)> TransformAction::scale = [](SceneNode* node, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    transform.scale();
    if (type == ActionType::RELATIVE && parent) { scale(parent, type); }
  };

  Action<void(SceneNode*, float, glm::vec3, ActionType)> TransformAction::set_rotation =
      [](SceneNode* node, float angle, glm::vec3 axis, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent) { transform.m_rotation = glm::angleAxis(angle, esp::normalize(axis)); }
    else { transform.m_rotation = glm::angleAxis(angle, esp::normalize(axis)) * get_rotation(parent, type); }

    transform.rotate();
  };

  Action<void(SceneNode*, float, glm::vec3, ActionType)> TransformAction::update_rotation =
      [](SceneNode* node, float angle, glm::vec3 axis, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent)
    {
      transform.m_rotation *= glm::angleAxis(angle, esp::normalize(axis));
    }
    else { transform.m_rotation *= glm::angleAxis(angle, esp::normalize(axis)) * get_rotation(parent, type); }

    transform.rotate();
  };

  Action<glm::quat(SceneNode*, ActionType)> TransformAction::get_rotation = [](SceneNode* node, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    if (type == ActionType::ABSOLUTE || !parent) { return transform.m_rotation; }

    return transform.m_rotation * get_rotation(parent, type);
  };

  Action<void(SceneNode*, ActionType)> TransformAction::rotate = [](SceneNode* node, ActionType type)
  {
    auto& transform = get_component(node);
    auto parent     = node->get_parent();

    transform.rotate();
    if (type == ActionType::RELATIVE && parent) { rotate(parent, type); }
  };

  TransformComponent& TransformAction::get_component(SceneNode* node)
  {
    return node->get_entity()->get_component<TransformComponent>();
  }

} // namespace esp