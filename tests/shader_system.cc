#include <catch2/catch_test_macros.hpp>

#include "Core/Logger.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Systems/ShaderSystem.hh"

TEST_CASE("Shader system", "[shader_system]")
{
  auto logger          = esp::Logger::create();
  fs::path shader_path = fs::current_path() / ".." / "tests" / "shaders";

  {
    auto shader_system = esp::ShaderSystem::create(shader_path);
    REQUIRE(shader_system->get_shader_base_path() == shader_path);
  }
}