#ifndef ESPERT_CORESYSTEMS_CLIENTPOINT_HH_
#define ESPERT_CORESYSTEMS_CLIENTPOINT_HH_

#include "Application.hh"
#include "ApplicationContext.hh"

extern Espert::Application* Espert::createAppInstance();

int main(int argc, char** argv)
{
  auto context                      = Espert::ApplicationContext::create();
  Espert::Application* app_instance = Espert::createAppInstance();

  /* running */
  {
    app_instance->set_context(std::move(context));
    app_instance->run();
  }

  delete app_instance;
}

#endif // ESPERT_CORESYSTEMS_CLIENTPOINT_HH_
