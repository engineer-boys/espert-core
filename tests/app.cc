#include "Core/Application.hh"
#include "Core/ApplicationContext.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include <catch2/catch_test_macros.hpp>
#include <thread>

class TestApp : public esp::Application
{
 private:
 public:
  TestApp() {}

  void set_background_color() { m_frame_manager->set_clear_color(glm::vec4{ .1f, .1f, .3f, 1.f }); }
  void terminate()
  {
    esp::WindowClosedEvent e;
    events_manager(e);
  }
};

esp::Application* esp::create_app_instance()
{
  const auto& app = new TestApp();
  app->set_background_color();
  return app;
}

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