#include <spdlog/sinks/stdout_color_sinks.h>

#include "Logger.hh"

namespace Espert
{
Logger* Logger::_s_instance = nullptr;

Logger::Logger()
{
  if (Logger::_s_instance != nullptr)
  {
    throw std::runtime_error("The logger instance already exists!");
  }

  Logger::_s_instance = this;
}

Logger::~Logger() { Logger::_s_instance = nullptr; }

std::unique_ptr<Logger> Logger::create()
{
  /* create singleton */
  auto logger = std::unique_ptr<Logger>{ new Logger() };

  /* set pattern of messages */
  spdlog::set_pattern("%^[%T][%n]%$ : %v");

  /* set multithread logger for trace level */
  logger->_m_core_logger = spdlog::stdout_color_mt("Espert");
  logger->_m_core_logger->set_level(spdlog::level::trace);

  /* set multithread logger for client level */
  logger->_m_client_logger = spdlog::stdout_color_mt("Client");
  logger->_m_client_logger->set_level(spdlog::level::trace);

  ESP_CORE_INFO("Logger is created");
  return logger;
}
} // namespace Espert
