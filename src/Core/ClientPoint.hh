#ifndef ESPERT_CORE_CLIENTPOINT_HH
#define ESPERT_CORE_CLIENTPOINT_HH

#include "EspApplication.hh"
#include "EspApplicationContext.hh"

/// @brief Creates instance of the app. (to be defined by the user0
/// @return Pointer to created app.
extern esp::EspApplication* esp::create_app_instance();

int main(int argc, char** argv)
{
  auto context                      = esp::EspApplicationContext::create();
  esp::EspApplication* app_instance = esp::create_app_instance();

  /* running */
  {
    app_instance->set_context(std::move(context));
    app_instance->run();
  }

  delete app_instance;
}

#endif // ESPERT_CORE_CLIENTPOINT_HH
