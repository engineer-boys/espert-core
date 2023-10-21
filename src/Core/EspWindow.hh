#ifndef ESPERT_CORE_ESPERTWINDOW_HH
#define ESPERT_CORE_ESPERTWINDOW_HH

#include "Core/Events/Event.hh"
#include "esppch.hh"

// libs
#include "volk.h"

#include "GLFW/glfw3.h"

namespace esp
{
  class EspWindow
  {
   public:
    struct WindowData
    {
     public:
      using EventManagerFun = std::function<void(Event&)>;

     public:
      std::string m_title;
      unsigned int m_width;
      unsigned int m_height;
      EventManagerFun m_events_manager_fun;

      WindowData(const std::string title = "Espert window", unsigned int width = 1280, unsigned int height = 720) :
          m_title(title), m_width(width), m_height(height)
      {
      }
    };

   private:
    static bool s_is_exist;

   private:
    WindowData m_data;
    GLFWwindow* m_window;

   private:
    void init(const WindowData& data);
    void destroy();
    void set_callbacks();

    EspWindow(const WindowData& data);

   public:
    ~EspWindow();

    void update();

    inline unsigned int get_width() { return m_data.m_width; }
    inline unsigned int get_height() { return m_data.m_height; }
    inline VkExtent2D get_extent() const
    {
      return { static_cast<uint32_t>(m_data.m_height), static_cast<uint32_t>(m_data.m_height) };
    }
    inline void set_events_manager_fun(const WindowData::EventManagerFun& callback)
    {
      m_data.m_events_manager_fun = callback;
    }

    static std::unique_ptr<EspWindow> create(const WindowData& data);

    void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);
  };
} // namespace esp

#endif // ESPERT_CORE_ESPERTWINDOW_HH
