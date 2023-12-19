#ifndef ESPERT_CORE_ESP_WINDOW_HH
#define ESPERT_CORE_ESP_WINDOW_HH

#include "Core/Events/Event.hh"
#include "esppch.hh"

#include "GLFW/glfw3.h"

namespace esp
{
  class EspWindow
  {
    /* -------------------------- INNER STRUCTURE -------------------------- */
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

      const bool m_disable_cursor = false;

      WindowData(const std::string title, unsigned int width, unsigned int height) :
          m_title(title), m_width(width), m_height(height)
      {
      }
    };

    /* -------------------------- FIELDS ----------------------------------- */
   private:
    static EspWindow* s_instance;

    // std::array<int, 6> m_camera_keys = { GLFW_KEY_W, GLFW_KEY_S,     GLFW_KEY_A,
    //                                      GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT };

    std::unique_ptr<WindowData> m_data;
    GLFWwindow* m_window;

    /* -------------------------- METHODS ---------------------------------- */
   private:
    void set_callbacks();
    // void handle_camera_key_presses();

   public:
    EspWindow(WindowData* data);
    ~EspWindow();

    void init();
    void terminate();
    void update();
    void create_window_surface();

    inline unsigned int get_width() { return m_data->m_width; }
    inline unsigned int get_height() { return m_data->m_height; }

    inline void set_events_manager_fun(const WindowData::EventManagerFun& callback)
    {
      m_data->m_events_manager_fun = callback;
    }

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspWindow> create(WindowData* data);

    inline static void get_framebuffer_size(int& width, int& height)
    {
      glfwGetFramebufferSize(s_instance->m_window, &width, &height);
    }
  };
} // namespace esp

#endif // ESPERT_CORE_ESP_WINDOW_HH
