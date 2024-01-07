#include "tests/test_app.hh"

#include <catch2/catch_test_macros.hpp>
#include <thread>

TEST_CASE("App - run", "[app]")
{
  auto context                      = esp::EspApplicationContext::create();
  esp::EspApplication* app_instance = esp::create_app_instance();
  auto test_app                     = dynamic_cast<TestApp*>(app_instance);

  {
    test_app->set_context(std::move(context));
    std::thread app_thread(&TestApp::run, test_app);

    // ignore windows warning: invalid validation layer name for layers:
    // GalaxyOverlayVkLayer
    // GalaxyOverlayVkLayer_VERBOSE
    // GalaxyOverlayVkLayer_DEBUG
    if (WIN32) {
      REQUIRE(esp::EspDebugMessenger::get_warning_count() <= 3);
    }
    else {
      REQUIRE(esp::EspDebugMessenger::get_warning_count() == 0);
    }
    REQUIRE(esp::EspDebugMessenger::get_error_count() == 0);

    test_app->terminate();

    app_thread.join();
  }

  delete test_app;
}