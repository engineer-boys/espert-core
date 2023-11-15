#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "Core/Logger.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Systems/ResourceSystem.hh"

TEST_CASE("Resource system", "[resource_system]")
{
  auto logger         = esp::Logger::create();
  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";

  {
    auto resource_system = esp::ResourceSystem::create(asset_path);
    REQUIRE(resource_system->get_asset_base_path() == asset_path);
  }
}

TEST_CASE("Resource system - binary loader", "[resource_system]")
{
  auto logger         = esp::Logger::create();
  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";

  {
    auto resource_system = esp::ResourceSystem::create(asset_path);

    auto params = esp::BinaryResourceParams();

    auto resource = esp::ResourceSystem::load<esp::BinaryResource>("bin/test.bin", params);

    REQUIRE(strcmp(static_cast<const char*>(resource->get_data()), "test") == 0);

    esp::ResourceSystem::unload(std::move(resource));
  }
}

TEST_CASE("Resource system - text loader", "[resource_system]")
{
  auto logger         = esp::Logger::create();
  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";

  {
    auto resource_system = esp::ResourceSystem::create(asset_path);

    auto params = esp::TextResourceParams();

    auto resource = esp::ResourceSystem::load<esp::TextResource>("test.txt", params);
    std::unique_ptr<esp::TextResource> text_resource =
        std::unique_ptr<esp::TextResource>(dynamic_cast<esp::TextResource*>(resource.release()));

    REQUIRE(strcmp(static_cast<const char*>(text_resource->get_data()), "test") == 0);
    REQUIRE(text_resource->get_num_of_lines() == 1);

    esp::ResourceSystem::unload(std::move(text_resource));
  }
}

TEST_CASE("Resource system - image loader", "[resource_system]")
{
  auto logger         = esp::Logger::create();
  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";

  {
    auto resource_system = esp::ResourceSystem::create(asset_path);

    auto image_params = esp::ImageResourceParams();
    auto resource     = esp::ResourceSystem::load<esp::ImageResource>("test.jpg", image_params);
    std::unique_ptr<esp::ImageResource> image_resource =
        std::unique_ptr<esp::ImageResource>(dynamic_cast<esp::ImageResource*>(resource.release()));

    auto binary_params = esp::BinaryResourceParams();
    resource           = esp::ResourceSystem::load<esp::BinaryResource>("test.jpg.bin", binary_params);
    std::unique_ptr<esp::BinaryResource> binary_resource =
        std::unique_ptr<esp::BinaryResource>(dynamic_cast<esp::BinaryResource*>(resource.release()));

    REQUIRE(memcmp(static_cast<const char*>(image_resource->get_data()),
                   static_cast<const char*>(binary_resource->get_data()),
                   image_resource->get_size()) == 0);
    REQUIRE(image_resource->get_width() == 700);
    REQUIRE(image_resource->get_height() == 576);
    REQUIRE(image_resource->get_channel_count() == 3);

    esp::ResourceSystem::unload(std::move(image_resource));
    esp::ResourceSystem::unload(std::move(binary_resource));
  }
}

class TestResource : public esp::Resource
{
 public:
  TestResource(const fs::path& path, uint64_t data_size, resource_data_t data) :
      Resource(path, data_size, std::move(data))
  {
  }

  PREVENT_COPY(TestResource);
};

struct TestResourceParams : public esp::ResourceParams
{
};

class TestLoader : public esp::Loader
{
 public:
  inline virtual std::unique_ptr<esp::Resource> load(const fs::path& path, const esp::ResourceParams& params) override
  {
    fs::path full_path = esp::ResourceSystem::get_asset_base_path() / path;

    void* data = (void*)calloc(sizeof(char), 5);
    ESP_ASSERT(data != nullptr, "Could not allocate memory for {}.", full_path.string());

    memcpy(data, "test", 5);

    return std::unique_ptr<esp::Resource>(new TestResource(full_path, 5, resource_data_t(data, VOID_DELETER)));
  }
  inline virtual void unload(std::unique_ptr<esp::Resource> resource) override { resource.reset(nullptr); }
};

TEST_CASE("Resource system - custom loader", "[resource_system]")
{
  auto logger         = esp::Logger::create();
  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";

  {
    auto resource_system = esp::ResourceSystem::create(asset_path);

    esp::ResourceSystem::register_loader<TestResource>(std::unique_ptr<esp::Loader>(new TestLoader()));

    auto params   = TestResourceParams();
    auto resource = esp::ResourceSystem::load<TestResource>("Test", params);

    REQUIRE(strcmp(static_cast<const char*>(resource->get_data()), "test") == 0);

    esp::ResourceSystem::unload(std::move(resource));
  }
}