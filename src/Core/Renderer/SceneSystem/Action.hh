#ifndef SCENE_SCENE_ACTIONS_HH
#define SCENE_SCENE_ACTIONS_HH

#include "Core/Renderer/SceneSystem/Components/Components.hh"

#include "esppch.hh"

namespace esp
{
  /* -------------------- Action template -------------------- */
  template<typename Func> struct Action_t;

  template<typename T, typename... Args> struct Action_t<T(Args...)>
  {
    using type = std::function<T(Args...)>;
  };

  template<typename Func> using Action = typename Action_t<Func>::type;
  /* -------------------------------------------------------- */

  class Node;

  namespace action
  {
    enum ActionType
    {
      ABSOLUTE,
      RELATIVE
    };

    // TODO: try to add default action types
    struct TransformAction
    {
      static Action<void(Node*)> reset;

      static Action<void(Node*, glm::vec3, ActionType)> set_translation;
      static Action<void(Node*, glm::vec3, ActionType)> update_translation;
      static Action<void(Node*, ActionType)> translate;

      static Action<void(Node*, float, ActionType)> set_scale;
      static Action<void(Node*, float, ActionType)> update_scale;
      static Action<void(Node*, ActionType)> scale;

      // TODO: try to separate axis and self rotations
      static Action<void(Node*, float, glm::vec3, ActionType)> set_rotation;
      static Action<void(Node*, float, glm::vec3, ActionType)> update_rotation;
      static Action<void(Node*, ActionType)> rotate;

     private:
      static TransformComponent& get_transform(Node* node);
    };
  } // namespace action
} // namespace esp

#endif // SCENE_SCENE_ACTIONS_HH
