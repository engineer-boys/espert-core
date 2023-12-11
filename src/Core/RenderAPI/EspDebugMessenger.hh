#ifndef RENDER_API_ESP_DEBUG_MESSENGER_HH
#define RENDER_API_ESP_DEBUG_MESSENGER_HH

#include "esppch.hh"

namespace esp
{
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
    virtual ~EspDebugMessenger() = default;
    PREVENT_COPY(EspDebugMessenger);

    static std::unique_ptr<EspDebugMessenger> create();

    inline static EspDebugMessenger* get_instance() { return EspDebugMessenger::s_instance; }
    static inline const uint32_t get_error_count() { return s_instance->m_error_count; }
    static inline const uint32_t get_error_and_warning_count()
    {
      return s_instance->m_error_count + s_instance->m_warning_count;
    }
    static inline const uint32_t get_warning_count() { return s_instance->m_warning_count; }
    static inline const uint32_t get_info_count() { return s_instance->m_info_count; }

    virtual void terminate() = 0;
    virtual void init()      = 0;
  };

} // namespace esp

#endif // RENDER_API_ESP_DEBUG_MESSENGER_HH