#include <catch2/catch_test_macros.hpp>

#include "Core/Logger.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Systems/ResourceSystem.hh"
#include "Core/Systems/TextureSystem.hh"

TEST_CASE("Texture system", "[texture_system]")
{
  auto logger = esp::Logger::create();

  REQUIRE_FALSE(esp::TextureSystem::is_initialized());

  auto texture_system = esp::TextureSystem::init();

  REQUIRE(esp::TextureSystem::is_initialized);

  REQUIRE(texture_system->get_default_texture() != nullptr);
  auto default_texture = texture_system->get_default_texture();
  REQUIRE(default_texture->get_channel_count() == 4);
  REQUIRE(default_texture->get_width() == 32);
  REQUIRE(default_texture->get_height() == 32);
  REQUIRE(default_texture->get_mip_levels() == 1);

  REQUIRE(texture_system->get_default_diffuse_texture() != nullptr);
  auto default_diffuse_texture = texture_system->get_default_diffuse_texture();
  REQUIRE(default_diffuse_texture->get_channel_count() == 4);
  REQUIRE(default_diffuse_texture->get_width() == 32);
  REQUIRE(default_diffuse_texture->get_height() == 32);
  REQUIRE(default_diffuse_texture->get_mip_levels() == 1);

  REQUIRE(texture_system->get_default_specular_texture() != nullptr);
  auto default_specular_texture = texture_system->get_default_specular_texture();
  REQUIRE(default_specular_texture->get_channel_count() == 4);
  REQUIRE(default_specular_texture->get_width() == 32);
  REQUIRE(default_specular_texture->get_height() == 32);
  REQUIRE(default_specular_texture->get_mip_levels() == 1);

  REQUIRE(texture_system->get_default_normal_texture() != nullptr);
  auto default_normal_texture = texture_system->get_default_normal_texture();
  REQUIRE(default_normal_texture->get_channel_count() == 4);
  REQUIRE(default_normal_texture->get_width() == 32);
  REQUIRE(default_normal_texture->get_height() == 32);
  REQUIRE(default_normal_texture->get_mip_levels() == 1);

  texture_system->shutdown();
  REQUIRE_FALSE(esp::TextureSystem::is_initialized());
}

TEST_CASE("Texture system - load texture", "[texture_system]")
{
  fs::path asset_path  = fs::current_path() / ".." / "tests" / "assets";
  auto logger          = esp::Logger::create();
  auto resource_system = esp::ResourceSystem::init(asset_path);
  auto texture_system  = esp::TextureSystem::init();

  auto texture = esp::TextureSystem::acquire("test.jpg");
  REQUIRE(texture->get_name() == "test.jpg");
  REQUIRE(texture->get_channel_count() == 3);
  REQUIRE(texture->get_width() == 700);
  REQUIRE(texture->get_height() == 576);
  REQUIRE(texture->get_mip_levels() == 10);

  texture_system->release("test.jpg");

  texture_system->shutdown();
  resource_system->shutdown();
}