#include <catch2/catch_test_macros.hpp>

#include "Core/Logger.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Systems/ShaderSystem.hh"

TEST_CASE("Shader system", "[shader_system]")
{
  auto logger = esp::Logger::create();

  {
    auto shader_system = esp::ShaderSystem::create();
  }
}