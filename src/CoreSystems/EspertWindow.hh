#ifndef ESPERT_CORESYSTEMS_ESPERTWINDOW_HH_
#define ESPERT_CORESYSTEMS_ESPERTWINDOW_HH_

#include "Events/Event.hh"
#include "GLFW/glfw3.h"
#include "esppch.hh"

namespace Espert
{
  class EspertWindow
  {
   public:
    struct WindowData
    {
     public:
      using EventManagerFun = std::function<void(Event&)>;

     public:
      std::string title;
      unsigned int width;
      unsigned int height;
      EventManagerFun events_manager_fun;

      WindowData(const std::string title = "Espert widnow",
                 unsigned int width = 1280, unsigned int height = 720) :
          title(title),
          width(width), height(height)
      {
      }
    };

   private:
    static bool _s_is_exist;

   private:
    WindowData _m_data;
    GLFWwindow* _m_window;

   private:
    void init(const WindowData& data);
    void destroy();

    EspertWindow(const WindowData& data);

   public:
    ~EspertWindow();

    void update();

    inline unsigned int get_width() { return _m_data.width; }
    inline unsigned int get_height() { return _m_data.height; }
    inline void
    set_events_manager_fun(const WindowData::EventManagerFun& callback)
    {
      _m_data.events_manager_fun = callback;
    }

    static std::unique_ptr<EspertWindow> create(const WindowData& data);
  };
} // namespace Espert

#endif // ESPERT_CORESYSTEMS_ESPERTWINDOW_HH_