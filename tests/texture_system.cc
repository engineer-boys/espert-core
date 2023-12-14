#include "tests/test_app.hh"

#include "Core/Logger.hh"
#include "Core/RenderAPI/Resources/EspTexture.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Systems/ResourceSystem.hh"
#include "Core/Systems/TextureSystem.hh"
#include "Platform/Vulkan/Resources/VulkanTexture.hh"
#include <catch2/catch_test_macros.hpp>
#include <thread>

TEST_CASE("Texture system", "[texture_system]")
{
  auto context                   = esp::ApplicationContext::create();
  esp::Application* app_instance = esp::create_app_instance();
  auto test_app                  = dynamic_cast<TestApp*>(app_instance);

  {
    test_app->set_context(std::move(context));
    std::thread app_thread(&TestApp::run, test_app);

    REQUIRE(esp::TextureSystem::get_default_albedo_texture() != nullptr);
    auto default_albedo_texture = esp::TextureSystem::get_default_albedo_texture();
    REQUIRE(default_albedo_texture->get_channel_count() == 4);
    REQUIRE(default_albedo_texture->get_width() == 32);
    REQUIRE(default_albedo_texture->get_height() == 32);
    REQUIRE(default_albedo_texture->get_mip_levels() == 6);

    REQUIRE(esp::TextureSystem::get_default_normal_texture() != nullptr);
    auto default_normal_texture = esp::TextureSystem::get_default_normal_texture();
    REQUIRE(default_normal_texture->get_channel_count() == 4);
    REQUIRE(default_normal_texture->get_width() == 32);
    REQUIRE(default_normal_texture->get_height() == 32);
    REQUIRE(default_normal_texture->get_mip_levels() == 6);

    REQUIRE(esp::TextureSystem::get_default_metallic_texture() != nullptr);
    auto default_metallic_texture = esp::TextureSystem::get_default_metallic_texture();
    REQUIRE(default_metallic_texture->get_channel_count() == 4);
    REQUIRE(default_metallic_texture->get_width() == 32);
    REQUIRE(default_metallic_texture->get_height() == 32);
    REQUIRE(default_metallic_texture->get_mip_levels() == 6);

    REQUIRE(esp::TextureSystem::get_default_roughness_texture() != nullptr);
    auto default_roughness_texture = esp::TextureSystem::get_default_roughness_texture();
    REQUIRE(default_roughness_texture->get_channel_count() == 4);
    REQUIRE(default_roughness_texture->get_width() == 32);
    REQUIRE(default_roughness_texture->get_height() == 32);
    REQUIRE(default_roughness_texture->get_mip_levels() == 6);

    REQUIRE(esp::TextureSystem::get_default_ao_texture() != nullptr);
    auto default_ao_texture = esp::TextureSystem::get_default_ao_texture();
    REQUIRE(default_ao_texture->get_channel_count() == 4);
    REQUIRE(default_ao_texture->get_width() == 32);
    REQUIRE(default_ao_texture->get_height() == 32);
    REQUIRE(default_ao_texture->get_mip_levels() == 6);

    test_app->terminate();

    app_thread.join();
  }

  delete test_app;
}

TEST_CASE("Texture system - load texture", "[texture_system]")
{
  auto context                   = esp::ApplicationContext::create();
  esp::Application* app_instance = esp::create_app_instance();
  auto test_app                  = dynamic_cast<TestApp*>(app_instance);

  {
    test_app->set_context(std::move(context));
    std::thread app_thread(&TestApp::run, test_app);

    auto texture = esp::TextureSystem::acquire("test.jpg");
    REQUIRE(texture->get_name() == "test.jpg");
    REQUIRE(texture->get_channel_count() == 3);
    REQUIRE(texture->get_width() == 700);
    REQUIRE(texture->get_height() == 576);
    REQUIRE(texture->get_mip_levels() == 10);

    esp::TextureSystem::release("test.jpg");

    test_app->terminate();

    app_thread.join();
  }

  delete test_app;
}