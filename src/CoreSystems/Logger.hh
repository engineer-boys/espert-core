#ifndef __ENGINE__LOGGER_HH__
#define __ENGINE__LOGGER_HH__

#include "esppch.hh"
#include <spdlog/spdlog.h>

namespace Esper
{
    class Logger
    {
    private:
        static Logger *_s_instance;

    private:
        std::shared_ptr<spdlog::logger> _m_core_logger;
        std::shared_ptr<spdlog::logger> _m_client_logger;

        Logger();

    public:
        ~Logger();
        static std::unique_ptr<Logger> create();

        inline static Logger *get_instance() { return Logger::_s_instance; }
        inline static std::shared_ptr<spdlog::logger> &get_core_logger() { return Logger::_s_instance->_m_core_logger; }
        inline static std::shared_ptr<spdlog::logger> &get_client_logger() { return Logger::_s_instance->_m_client_logger; }
    };

} // namespace Esper

/* ################## LOGGER MACROS ################################# */
#ifdef NDEBUG
/* FOR CORE */
#define EP_CORE_TRACE(...)
#define EP_CORE_INFO(...)
#define EP_CORE_WARM(...)
#define EP_CORE_ERROR(...)
#define EP_CORE_CRITICAL(...)

/* FOR CLIENT */
#define EP_TRACE(...)
#define EP_INFO(...)
#define EP_WARM(...)
#define EP_ERROR(...)
#define EP_CRITICAL(...)
#else
/* FOR CORE */
#define EP_CORE_TRACE(...) Esper::Logger::get_core_logger()->trace(__VA_ARGS__)
#define EP_CORE_INFO(...) Esper::Logger::get_core_logger()->info(__VA_ARGS__)
#define EP_CORE_WARM(...) Esper::Logger::get_core_logger()->warm(__VA_ARGS__)
#define EP_CORE_ERROR(...) Esper::Logger::get_core_logger()->error(__VA_ARGS__)
#define EP_CORE_CRITICAL(...) Esper::Logger::get_core_logger()->critical(__VA_ARGS__)

/* FOR CLIENT */
#define EP_TRACE(...) Esper::Logger::get_client_logger()->trace(__VA_ARGS__)
#define EP_INFO(...) Esper::Logger::get_client_logger()->info(__VA_ARGS__)
#define EP_WARM(...) Esper::Logger::get_client_logger()->warm(__VA_ARGS__)
#define EP_ERROR(...) Esper::Logger::get_client_logger()->error(__VA_ARGS__)
#define EP_CRITICAL(...) Esper::Logger::get_client_logger()->critical(__VA_ARGS__)
#endif

#endif /* __ENGINE__LOGGER_HH__ */
