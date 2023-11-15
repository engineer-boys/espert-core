#include "ApplicationContext.hh"

namespace esp
{
  bool ApplicationContext::s_is_exist = false;

  ApplicationContext::ApplicationContext()
  {
    if (ApplicationContext::s_is_exist) { throw std::runtime_error("The application contex already exists!"); }

    ApplicationContext::s_is_exist = true;
  }

  ApplicationContext::~ApplicationContext() { ApplicationContext::s_is_exist = false; }

  std::unique_ptr<ApplicationContext> ApplicationContext::create()
  {
    auto context = std::unique_ptr<ApplicationContext>{ new ApplicationContext() };

    /* create context dependencies */
    {
      context->m_logger          = Logger::create();
      context->m_resource_system = ResourceSystem::create(fs::current_path().parent_path() / "resources");
      context->m_texture_system  = TextureSystem::create();
    }
    /* end of creating context dependencies */

    return context;
  }

} // namespace esp
