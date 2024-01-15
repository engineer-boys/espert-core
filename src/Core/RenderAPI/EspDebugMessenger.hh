#ifndef RENDER_API_ESP_DEBUG_MESSENGER_HH
#define RENDER_API_ESP_DEBUG_MESSENGER_HH

#include "esppch.hh"

namespace esp
{
  /// @brief Responsible for logging and counting graphic's API debug messages.
  class EspDebugMessenger
  {
   protected:
    static EspDebugMessenger* s_instance;
    uint32_t m_error_count;
    uint32_t m_warning_count;
    uint32_t m_info_count;

   protected:
    EspDebugMessenger();

   public:
    /// @brief Virtual destructor.
    virtual ~EspDebugMessenger() = default;
    PREVENT_COPY(EspDebugMessenger);

    /// @brief Creates instance of EspDebugMessenger.
    /// @return Unique pointer to instance of EspDebugMessenger.
    static std::unique_ptr<EspDebugMessenger> create();

    /// @brief Returns pointer to instance of EspDebugMessenger.
    /// @return Pointer to instance of EspDebugMessenger.
    inline static EspDebugMessenger* get_instance() { return EspDebugMessenger::s_instance; }
    /// @brief Returns number of reveiced errors.
    /// @return Number of reveiced errors.
    static inline const uint32_t get_error_count() { return s_instance->m_error_count; }
    /// @brief Returns sum of reveiced errors and warnings.
    /// @return Sum of reveiced errors and warnings.
    static inline const uint32_t get_error_and_warning_count()
    {
      return s_instance->m_error_count + s_instance->m_warning_count;
    }
    /// @brief Returns number of reveiced warnings.
    /// @return Number of reveiced warnings.
    static inline const uint32_t get_warning_count() { return s_instance->m_warning_count; }
    /// @brief Returns number of reveiced infos.
    /// @return Number of reveiced infos.
    static inline const uint32_t get_info_count() { return s_instance->m_info_count; }

    /// @brief Termiantes the debug messenger.
    virtual void terminate() = 0;
    /// @brief Initializes the debug messenger.
    virtual void init() = 0;
  };

} // namespace esp

#endif // RENDER_API_ESP_DEBUG_MESSENGER_HH