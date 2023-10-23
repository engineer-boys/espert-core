#include "ApplicationContext.hh"

namespace esp
{
  bool ApplicationContext::_s_is_exist = false;

  ApplicationContext::ApplicationContext()
  {
    if (ApplicationContext::_s_is_exist)
    {
      throw std::runtime_error("The application contex already exists!");
    }

    /* create context dependencies */
    {
      _m_logger       = Logger::create();
      _m_glfw_context = GLFWContext::create();
    }
    /* end of creating context dependencies */

    ApplicationContext::_s_is_exist = true;
  }

  ApplicationContext::~ApplicationContext()
  {
    ApplicationContext::_s_is_exist = false;
    /*
      They will be deleted by unique_ptr automatically:
      - logger
      - glfw context
    */
  }

  std::unique_ptr<ApplicationContext> ApplicationContext::create()
  {
    return std::unique_ptr<ApplicationContext>{ new ApplicationContext() };
  }

} // namespace esp
