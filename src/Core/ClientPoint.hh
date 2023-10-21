#ifndef ESPERT_CORE_CLIENTPOINT_HH
#define ESPERT_CORE_CLIENTPOINT_HH

#include "Application.hh"
#include "ApplicationContext.hh"

extern esp::Application* esp::create_app_instance();

int main(int argc, char** argv)
{
  auto context                   = esp::ApplicationContext::create();
  esp::Application* app_instance = esp::create_app_instance();

  /* running */
  {
    app_instance->set_context(std::move(context));
    app_instance->run();
  }

  delete app_instance;
}

#endif // ESPERT_CORE_CLIENTPOINT_HH
