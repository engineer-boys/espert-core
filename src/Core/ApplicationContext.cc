#include "ApplicationContext.hh"

namespace esp
{
  ApplicationContext* ApplicationContext::s_instance = nullptr;

  ApplicationContext::ApplicationContext()
  {
    ESP_ASSERT(ApplicationContext::s_instance == nullptr, "The application contex already exists!");
    ApplicationContext::s_instance = this;
  }

  ApplicationContext::~ApplicationContext()
  {
    ESP_ASSERT(ApplicationContext::s_instance != nullptr, "The application contex is deleted twice!");
    ApplicationContext::s_instance = nullptr;
  }

  std::unique_ptr<ApplicationContext> ApplicationContext::create()
  {
    auto context = std::unique_ptr<ApplicationContext>{ new ApplicationContext() };

    /* create context dependencies */
    {
      context->m_logger = Logger::create();
    }
    /* end of creating context dependencies */

    return context;
  }

} // namespace esp
