#ifndef ESPERT_CORE_INPUT_HH
#define ESPERT_CORE_INPUT_HH

#include "esppch.hh"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace esp
{
  /// @brief An interface for checking state of mouse and keyboard.
  class EspInput
  {
   public:
    /// @brief Checks wheter a certaing keyboard key is currently pressed.
    /// @param keycode Integer code of keyboard key.
    /// @return True if said keyboard key is currently pressed. False otherwise.
    static bool is_key_pressed(int keycode);
    /// @brief Checks wheter a certaing mouse key is currently pressed.
    /// @param keycode Integer code of mouse key.
    /// @return True if said mouse key is currently pressed. False otherwise.
    static bool is_mouse_button_pressed(int button);
    /// @brief Checks and returns current mouse position in x axis.
    /// @return Current mouse position in x axis.
    static float get_mouse_x();
    /// @brief Checks and returns current mouse position in y axis.
    /// @return Current mouse position in y axis.
    static float get_mouse_y();
  };
} // namespace esp

#endif // ESPERT_CORE_INPUT_HH
