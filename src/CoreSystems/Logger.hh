#ifndef ESPERT_CORESYSTEMS_LOGGER_HH_
#define ESPERT_CORESYSTEMS_LOGGER_HH_

#include <memory>
#include <spdlog/spdlog.h>

namespace Espert
{
class Logger
{
 private:
  static Logger* _s_instance;

 private:
  std::shared_ptr<spdlog::logger> _m_core_logger;
  std::shared_ptr<spdlog::logger> _m_client_logger;

  Logger();

 public:
  ~Logger();
  static std::unique_ptr<Logger> create();

  inline static Logger* get_instance() { return Logger::_s_instance; }
  inline static std::shared_ptr<spdlog::logger>& get_core_logger()
  {
    return Logger::_s_instance->_m_core_logger;
  }
  inline static std::shared_ptr<spdlog::logger>& get_client_logger()
  {
    return Logger::_s_instance->_m_client_logger;
  }
};

} // namespace Espert

/* ################## LOGGER MACROS ################################# */
#ifdef NDEBUG
/* FOR CORE */
#define ESP_CORE_TRACE(...)
#define ESP_CORE_INFO(...)
#define ESP_CORE_WARM(...)
#define ESP_CORE_ERROR(...)
#define ESP_CORE_CRITICAL(...)

/* FOR CLIENT */
#define ESP_TRACE(...)
#define ESP_INFO(...)
#define ESP_WARM(...)
#define ESP_ERROR(...)
#define ESP_CRITICAL(...)
#else
/* FOR CORE */
#define ESP_CORE_TRACE(...) \
  Espert::Logger::get_core_logger()->trace(__VA_ARGS__)
#define ESP_CORE_INFO(...) Espert::Logger::get_core_logger()->info(__VA_ARGS__)
#define ESP_CORE_WARM(...) Espert::Logger::get_core_logger()->warm(__VA_ARGS__)
#define ESP_CORE_ERROR(...) \
  Espert::Logger::get_core_logger()->error(__VA_ARGS__)
#define ESP_CORE_CRITICAL(...) \
  Espert::Logger::get_core_logger()->critical(__VA_ARGS__)

/* FOR CLIENT */
#define ESP_TRACE(...) Espert::Logger::get_client_logger()->trace(__VA_ARGS__)
#define ESP_INFO(...)  Espert::Logger::get_client_logger()->info(__VA_ARGS__)
#define ESP_WARM(...)  Espert::Logger::get_client_logger()->warm(__VA_ARGS__)
#define ESP_ERROR(...) Espert::Logger::get_client_logger()->error(__VA_ARGS__)
#define ESP_CRITICAL(...) \
  Espert::Logger::get_client_logger()->critical(__VA_ARGS__)
#endif

#endif // ESPERT_CORESYSTEMS_LOGGER_HH_
