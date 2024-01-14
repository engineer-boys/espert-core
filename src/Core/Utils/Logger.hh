#ifndef ESPERT_CORE_LOGGER_HH
#define ESPERT_CORE_LOGGER_HH

#include <memory>
#include <spdlog/spdlog.h>

namespace esp
{
  /// @brief Logs messages to the console.
  class Logger
  {
   private:
    static Logger* s_instance;

   private:
    std::shared_ptr<spdlog::logger> m_core_logger;
    std::shared_ptr<spdlog::logger> m_client_logger;

    Logger();

   public:
    /// @brief Destructor destroys instance of Logger.
    ~Logger();
    static std::unique_ptr<Logger> create();

    /// @brief Returns instance of Logger.
    /// @return Instance of Logger.
    inline static Logger* get_instance() { return Logger::s_instance; }
    /// @brief Returns reference to core logger.
    /// @return Reference to core logger.
    inline static std::shared_ptr<spdlog::logger>& get_core_logger() { return Logger::s_instance->m_core_logger; }
    /// @brief Returns reference to client logger.
    /// @return Reference to client logger.
    inline static std::shared_ptr<spdlog::logger>& get_client_logger() { return Logger::s_instance->m_client_logger; }
  };

} // namespace esp

/* ################## LOGGER MACROS ################################# */
#ifdef NDEBUG
/* FOR CORE */
#define ESP_CORE_TRACE(...)
#define ESP_CORE_INFO(...)
#define ESP_CORE_WARN(...)
#define ESP_CORE_ERROR(...)
#define ESP_CORE_CRITICAL(...)

/* FOR CLIENT */
#define ESP_TRACE(...)
#define ESP_INFO(...)
#define ESP_WARN(...)
#define ESP_ERROR(...)
#define ESP_CRITICAL(...)
#else
/* FOR CORE */
#define ESP_CORE_TRACE(...)    esp::Logger::get_core_logger()->trace(__VA_ARGS__)
#define ESP_CORE_INFO(...)     esp::Logger::get_core_logger()->info(__VA_ARGS__)
#define ESP_CORE_WARN(...)     esp::Logger::get_core_logger()->warn(__VA_ARGS__)
#define ESP_CORE_ERROR(...)    esp::Logger::get_core_logger()->error(__VA_ARGS__)
#define ESP_CORE_CRITICAL(...) esp::Logger::get_core_logger()->critical(__VA_ARGS__)

/* FOR CLIENT */
#define ESP_TRACE(...)         esp::Logger::get_client_logger()->trace(__VA_ARGS__)
#define ESP_INFO(...)          esp::Logger::get_client_logger()->info(__VA_ARGS__)
#define ESP_WARN(...)          esp::Logger::get_client_logger()->warn(__VA_ARGS__)
#define ESP_ERROR(...)         esp::Logger::get_client_logger()->error(__VA_ARGS__)
#define ESP_CRITICAL(...)      esp::Logger::get_client_logger()->critical(__VA_ARGS__)
#endif

#endif // ESPERT_CORE_LOGGER_HH
