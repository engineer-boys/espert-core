#include "Action.hh"
#include "Components/Components.hh"
#include "Entity.hh"
#include "SceneNode.hh"

namespace esp
{
  Action<glm::vec3(SceneNode*)> TransformActions::get_position = [](SceneNode* node)
  {
    auto& transform = node->get_entity()->get_component<TransformComponent>();

    if (!node->get_parent()) { return transform.get_translation(); }

    return transform.get_translation() + get_position(node->get_parent());
  };

  Action<void(SceneNode*, glm::vec3)> TransformActions::set_position = [](SceneNode* node, glm::vec3 pos)
  {
    auto& transform = node->get_entity()->get_component<TransformComponent>();

    node->get_parent() ? transform.set_translation(pos + get_position(node->get_parent()))
                       : transform.set_translation(pos);
  };
} // namespace esp