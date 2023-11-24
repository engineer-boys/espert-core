#ifndef SCENE_SCENE_ACTIONS_HH
#define SCENE_SCENE_ACTIONS_HH

#include "esppch.hh"

namespace esp
{

  template<typename Func> struct Action_t;

  template<typename T, typename... Args> struct Action_t<T(Args...)>
  {
    using type = std::function<T(Args...)>;
  };

  template<typename Func> using Action = typename Action_t<Func>::type;

  class SceneNode;

  struct TransformActions
  {
    static Action<glm::vec3(SceneNode*)> get_position;
    static Action<void(SceneNode*, glm::vec3)> set_position;
  };
} // namespace esp

#endif // SCENE_SCENE_ACTIONS_HH
