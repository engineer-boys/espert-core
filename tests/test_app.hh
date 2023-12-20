#ifndef ESPERT_TESTS_TEST_APP_HH
#define ESPERT_TESTS_TEST_APP_HH

#include "Core/EspApplication.hh"
#include "Core/EspApplicationContext.hh"

class TestApp : public esp::EspApplication
{
 private:
 public:
  TestApp() { m_resource_system->change_asset_base_path(fs::current_path() / ".." / "tests" / "assets"); }

  void terminate()
  {
    esp::WindowClosedEvent e;
    events_manager(e);
  }
};

inline esp::EspApplication* esp::create_app_instance()
{
  const auto& app = new TestApp();

  return app;
}

#endif // ESPERT_TESTS_TEST_APP_HH