#ifndef ESPERT_CORE_CLIENTPOINT_HH_
#define ESPERT_CORE_CLIENTPOINT_HH_

#include "Application.hh"
#include "ApplicationContext.hh"

extern esp::Application* esp::createAppInstance();

int main(int argc, char** argv)
{
  auto context                   = esp::ApplicationContext::create();
  esp::Application* app_instance = esp::createAppInstance();

  /* running */
  {
    app_instance->set_context(std::move(context));
    app_instance->run();
  }

  delete app_instance;
}

#endif // ESPERT_CORE_CLIENTPOINT_HH_
