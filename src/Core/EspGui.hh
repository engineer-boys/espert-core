#ifndef ESPERT_SANDBOX_ESPGUI_HH
#define ESPERT_SANDBOX_ESPGUI_HH

#include "esppch.hh"

namespace esp
{
  typedef int EspGuiWindowFlags;

  class EspGui
  {
   protected:
    std::string m_name;

   public:
    static bool m_use_gui;

   public:
    static std::unique_ptr<EspGui> create(const std::string& name = "EspGui");

    ~EspGui() = default;

    static void terminate();
    static void new_frame();
    static void begin(EspGuiWindowFlags flags = 0);
    static void end();
    static void end_frame();
    static void render();
  };

  enum EspGuiWindowFlags_
  {
    EspGuiWindowFlags_None                      = 0,
    EspGuiWindowFlags_NoTitleBar                = 1 << 0,
    EspGuiWindowFlags_NoResize                  = 1 << 1,
    EspGuiWindowFlags_NoMove                    = 1 << 2,
    EspGuiWindowFlags_NoScrollbar               = 1 << 3,
    EspGuiWindowFlags_NoScrollWithMouse         = 1 << 4,
    EspGuiWindowFlags_NoCollapse                = 1 << 5,
    EspGuiWindowFlags_AlwaysAutoResize          = 1 << 6,
    EspGuiWindowFlags_NoBackground              = 1 << 7,
    EspGuiWindowFlags_NoSavedSettings           = 1 << 8,
    EspGuiWindowFlags_NoMouseInputs             = 1 << 9,
    EspGuiWindowFlags_MenuBar                   = 1 << 10,
    EspGuiWindowFlags_HorizontalScrollbar       = 1 << 11,
    EspGuiWindowFlags_NoFocusOnAppearing        = 1 << 12,
    EspGuiWindowFlags_NoBringToFrontOnFocus     = 1 << 13,
    EspGuiWindowFlags_AlwaysVerticalScrollbar   = 1 << 14,
    EspGuiWindowFlags_AlwaysHorizontalScrollbar = 1 << 15,
    EspGuiWindowFlags_NoNavInputs               = 1 << 16,
    EspGuiWindowFlags_NoNavFocus                = 1 << 17,
    EspGuiWindowFlags_UnsavedDocument           = 1 << 18,
  };
} // namespace esp

#endif // ESPERT_SANDBOX_ESPGUI_HH
