#ifndef ESPERT_CORE_ESP_WINDOW_HH
#define ESPERT_CORE_ESP_WINDOW_HH

#include "Core/Events/Event.hh"
#include "esppch.hh"

#include "GLFW/glfw3.h"

namespace esp
{
  /// @brief Manages a window.
  class EspWindow
  {
    /* -------------------------- INNER STRUCTURE -------------------------- */
   public:
    /// @brief Structure containing basic window data.
    struct WindowData
    {
     public:
      using EventManagerFun = std::function<void(Event&)>;

     public:
      /// @brief Window title.
      std::string m_title;
      /// @brief Window width.
      const unsigned int m_width;
      /// @brief Window height.
      const unsigned int m_height;
      /// @brief Window event manager.
      EventManagerFun m_events_manager_fun;

      /// @brief Indicator if the cursor should be locked. (true if not locked, false otherwise)
      const bool m_disable_cursor = false;

      /// @brief Constructor setting basic window info.
      /// @param title Window title.
      /// @param width Window width.
      /// @param height Window height.
      /// @param disable_cursor Indicator if the cursor should be locked. (true if not locked, false otherwise)
      WindowData(const std::string title, unsigned int width, unsigned int height, bool disable_cursor) :
          m_title(title), m_width(width), m_height(height), m_disable_cursor(disable_cursor)
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
    /// @brief Constructor that sets window data.
    /// @param data Pointer to WindowData.
    EspWindow(WindowData* data);
    /// @brief Default destructor.
    ~EspWindow();

    /// @brief Initializes the window.
    void init();
    /// @brief Termiantes the window.
    void terminate();
    /// @brief Polls window events.
    void update();
    /// @brief Creates window surface to connect window to graphics API.
    void create_window_surface();

    /// @brief Returns window's width.
    /// @return Windows's width.
    inline unsigned int get_width() { return m_data->m_width; }
    /// @brief Returns window's height.
    /// @return Window's height.
    inline unsigned int get_height() { return m_data->m_height; }

    /// @brief Returns pointer to the window.
    /// @return Pointer to the window.
    inline GLFWwindow* get_window() const { return m_window; }

    /// @brief Sets window's event manager's callback.
    /// @param callback Event manager's callback to be set.
    inline void set_events_manager_fun(const WindowData::EventManagerFun& callback)
    {
      m_data->m_events_manager_fun = callback;
    }

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    /// @brief Creates window based on WindowData.
    /// @param data WindowData to create window width.
    /// @return Unique pointer to the created window.
    static std::unique_ptr<EspWindow> create(WindowData* data);

    /// @brief Returns frame buffer size based on window size.
    /// @param width Window's width.
    /// @param height Window's height.
    inline static void get_framebuffer_size(int& width, int& height)
    {
      glfwGetFramebufferSize(s_instance->m_window, &width, &height);
    }

    /// @brief Returns pointer to the instance of EspWindow.
    /// @return Pointer to the instance of EspWindow.
    static inline const EspWindow* get_instance() { return s_instance; }
  };
} // namespace esp

#endif // ESPERT_CORE_ESP_WINDOW_HH
