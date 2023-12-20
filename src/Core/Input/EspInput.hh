#ifndef ESPERT_CORE_INPUT_HH
#define ESPERT_CORE_INPUT_HH

#include "esppch.hh"

#include "GLFW/glfw3.h"

namespace esp
{
  class EspInput
  {
   public:
    static bool is_key_pressed(int keycode);

    static bool is_mouse_button_pressed(int button);
    static float get_mouse_x();
    static float get_mouse_y();
  };
} // namespace esp

#endif // ESPERT_CORE_INPUT_HH
