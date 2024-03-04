#ifndef ESPERT_CORE_ESP_WINDOW_HH
#define ESPERT_CORE_ESP_WINDOW_HH

#include "Core/Events/Event.hh"
#include "EspApplicationParams.hh"
#include "esppch.hh"

#define GLFW_INCLUDE_VULKAN
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
      /// @brief Window title.
      std::string m_title;
      /// @brief Window width.
      const uint32_t m_width;
      /// @brief Window height.
      const uint32_t m_height;
      /// @brief Window event manager.
      EventManagerFun m_events_manager_fun;

      /// @brief Indicator if the cursor should be locked. (true if not locked, false otherwise)
      const bool m_disable_cursor = false;

      /// @brief Window's presentation mode
      EspPresentationMode m_presentation_mode;

      /// @brief Constructor setting basic window info.
      /// @param title Window title.
      /// @param width Window width.
      /// @param height Window height.
      /// @param disable_cursor Indicator if the cursor should be locked. (true if not locked, false otherwise)
      WindowData(const std::string title,
                 uint32_t width,
                 uint32_t height,
                 bool disable_cursor,
                 EspPresentationMode presentation_mode) :
          m_title(title), m_width(width), m_height(height), m_disable_cursor(disable_cursor),
          m_presentation_mode(presentation_mode)
      {
      }
    };

    /* -------------------------- FIELDS ----------------------------------- */
   private:
    static EspWindow* s_instance;

    std::unique_ptr<WindowData> m_data;
    GLFWwindow* m_window;

    /* -------------------------- METHODS ---------------------------------- */
   private:
    void set_callbacks();
    void set_presentation_mode();

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
    inline uint32_t get_width() { return m_data->m_width; }
    /// @brief Returns window's height.
    /// @return Window's height.
    inline uint32_t get_height() { return m_data->m_height; }

    /// @brief Sets window's event manager's callback.
    /// @param callback Event manager's callback to be set.
    inline void set_events_manager_fun(const EventManagerFun& callback) { m_data->m_events_manager_fun = callback; }

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
    inline static const EspWindow* get_instance() { return s_instance; }

    /// @brief Returns pointer to the window.
    /// @return Pointer to the window.
    inline static GLFWwindow* get_window() { return s_instance->m_window; }

    inline static const EventManagerFun& get_event_manager_fun() { return s_instance->m_data->m_events_manager_fun; }
  };
} // namespace esp

#endif // ESPERT_CORE_ESP_WINDOW_HH
