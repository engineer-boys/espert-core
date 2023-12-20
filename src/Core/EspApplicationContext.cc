#include "EspApplicationContext.hh"

namespace esp
{
  EspApplicationContext* EspApplicationContext::s_instance = nullptr;

  EspApplicationContext::EspApplicationContext()
  {
    ESP_ASSERT(EspApplicationContext::s_instance == nullptr, "The application contex already exists!");
    EspApplicationContext::s_instance = this;
  }

  EspApplicationContext::~EspApplicationContext()
  {
    ESP_ASSERT(EspApplicationContext::s_instance != nullptr, "The application contex is deleted twice!");
    EspApplicationContext::s_instance = nullptr;
  }

  std::unique_ptr<EspApplicationContext> EspApplicationContext::create()
  {
    auto context = std::unique_ptr<EspApplicationContext>{ new EspApplicationContext() };

    /* create context dependencies */
    {
      context->m_logger = Logger::create();
    }
    /* end of creating context dependencies */

    return context;
  }

} // namespace esp
