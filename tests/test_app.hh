#ifndef ESPERT_TESTS_TEST_APP_HH
#define ESPERT_TESTS_TEST_APP_HH

#include "Core/EspApplication.hh"
#include "Core/EspApplicationContext.hh"

class TestApp : public esp::EspApplication
{
 public:
  TestApp() = default;

  void terminate()
  {
    esp::WindowClosedEvent e;
    events_manager(e);
  }
};

inline esp::EspApplication* esp::create_app_instance()
{
  TestApp::set_asset_base_path(fs::current_path() / ".." / "tests" / "assets");
  const auto& app = new TestApp();
  return app;
}

#endif // ESPERT_TESTS_TEST_APP_HH