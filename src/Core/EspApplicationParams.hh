#ifndef ESPERT_CORE_ESP_APPLICATION_PARAMS_HH
#define ESPERT_CORE_ESP_APPLICATION_PARAMS_HH

#include "esppch.hh"

namespace esp
{
  /// @brief Represents conditions for showing images to the screen
  enum EspPresentationMode
  {
    /// @brief Images submitted by application are transferred to the screen right away, which may result in tearing.
    ESP_PRESENT_MODE_IMMEDIATE_KHR = 0,
    /// @brief Images submitted by the application are queued for presentation, with the system discarding intermediate
    /// frames if the display is unable to keep pace, providing a tear-free experience. This is mode is not available on
    /// every device. If selected and not available, ESP_PRESENT_MODE_FIFO_KHR is used
    ESP_PRESENT_MODE_MAILBOX_KHR = 1,
    /// @brief Images submitted by the application are placed in a queue, with the presentation occurring at a fixed
    /// refresh rate. This mode is most similar to v-sync.
    ESP_PRESENT_MODE_FIFO_KHR = 2,
  };

  /// @brief Application set up parameters.
  struct EspApplicationParams
  {
    /// @brief Window title.
    std::string m_title = "Espert window";
    /// @brief Window width.
    unsigned int m_width = 1280;
    /// @brief Window height.
    unsigned int m_height = 720;
    /// @brief Flag for disabling window cursor.
    bool m_disable_cursor = false;
    /// @brief Presentation mode used for displaying images to the screen
    EspPresentationMode m_presentation_mode = EspPresentationMode::ESP_PRESENT_MODE_FIFO_KHR;

    bool m_use_gui = false;
  };

} // namespace esp

#endif // ESPERT_CORE_ESP_APPLICATION_PARAMS_HH
