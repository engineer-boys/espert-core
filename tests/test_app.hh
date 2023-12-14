#ifndef ESPERT_TESTS_TEST_APP_HH
#define ESPERT_TESTS_TEST_APP_HH

#include "Core/Application.hh"
#include "Core/ApplicationContext.hh"

class TestApp : public esp::Application
{
 private:
 public:
  TestApp() { m_resource_system->change_asset_base_path(fs::current_path() / ".." / "tests" / "assets"); }

  void set_background_color() { m_frame_manager->set_clear_color(glm::vec4{ .1f, .1f, .3f, 1.f }); }
  void terminate()
  {
    esp::WindowClosedEvent e;
    events_manager(e);
  }
};

inline esp::Application* esp::create_app_instance()
{
  const auto& app = new TestApp();
  app->set_background_color();
  return app;
}

#endif // ESPERT_TESTS_TEST_APP_HH