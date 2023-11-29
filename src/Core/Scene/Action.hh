#ifndef SCENE_SCENE_ACTIONS_HH
#define SCENE_SCENE_ACTIONS_HH

#include "Components/Components.hh"

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

  class SceneNode;

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
      static Action<void(SceneNode*, glm::vec3, ActionType)> set_translation;
      static Action<void(SceneNode*, glm::vec3, ActionType)> update_translation;
      static Action<glm::vec3(SceneNode*)> get_translation;
      static Action<void(SceneNode*, ActionType)> translate;

      static Action<void(SceneNode*, float, ActionType)> set_scale;
      static Action<void(SceneNode*, float, ActionType)> update_scale;
      static Action<float(SceneNode*)> get_scale;
      static Action<void(SceneNode*, ActionType)> scale;

      // TODO: try to separate axis and self rotations
      static Action<void(SceneNode*, float, glm::vec3, ActionType)> set_rotation;
      static Action<void(SceneNode*, float, glm::vec3, ActionType)> update_rotation;
      static Action<glm::quat(SceneNode*)> get_rotation;
      static Action<void(SceneNode*, ActionType)> rotate;

     private:
      static TransformComponent& get_component(SceneNode* node);
    };
  } // namespace action
} // namespace esp

#endif // SCENE_SCENE_ACTIONS_HH
