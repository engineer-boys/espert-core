#ifndef SCENE_ACTION_HH
#define SCENE_ACTION_HH

#include "esppch.hh"

namespace esp
{
  namespace action
  {
    /* -------------------- Action template -------------------- */
    template<typename Func> struct Action_t;

    template<typename T, typename... Args> struct Action_t<T(Args...)>
    {
      using type = std::function<T(Args...)>;
    };

    template<typename Func> using Action = typename Action_t<Func>::type;

    /* -------------------------------------------------------- */

    enum ActionType
    {
      ABSOLUTE,
      RELATIVE
    };
  } // namespace action
} // namespace esp

#endif // SCENE_ACTION_HH
