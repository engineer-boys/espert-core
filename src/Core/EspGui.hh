#ifndef ESPERT_SANDBOX_ESPGUI_HH
#define ESPERT_SANDBOX_ESPGUI_HH

#include "esppch.hh"

namespace esp
{
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
    static void begin();
    static void end();
    static void end_frame();
    static void render();
  };
} // namespace esp

#endif // ESPERT_SANDBOX_ESPGUI_HH
