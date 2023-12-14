#include "tests/test_app.hh"

#include <catch2/catch_test_macros.hpp>
#include <thread>

TEST_CASE("App - run", "[app]")
{
  auto context                   = esp::ApplicationContext::create();
  esp::Application* app_instance = esp::create_app_instance();
  auto test_app                  = dynamic_cast<TestApp*>(app_instance);

  {
    test_app->set_context(std::move(context));
    std::thread app_thread(&TestApp::run, test_app);

    // ignore warning: validation layers decrease performance
    REQUIRE(esp::EspDebugMessenger::get_warning_count() <= 1);
    REQUIRE(esp::EspDebugMessenger::get_error_count() == 0);

    test_app->terminate();

    app_thread.join();
  }

  delete test_app;
}