#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <iostream>
#include <string>

#include "Core/Logger.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Systems/ResourceSystem.hh"

namespace fs = std::filesystem;

TEST_CASE("Resource system", "[resource_system]")
{
  auto logger = esp::Logger::create();

  REQUIRE_FALSE(esp::ResourceSystem::is_initialized());

  fs::path asset_path  = fs::current_path() / ".." / "tests" / "assets";
  auto resource_system = esp::ResourceSystem::create(asset_path);

  REQUIRE(esp::ResourceSystem::is_initialized);
  REQUIRE(resource_system->get_asset_base_path() == asset_path);

  resource_system->terminate();
  REQUIRE_FALSE(esp::ResourceSystem::is_initialized());
}

TEST_CASE("Resource system - binary loader", "[resource_system]")
{
  auto logger = esp::Logger::create();

  fs::path asset_path  = fs::current_path() / ".." / "tests" / "assets";
  auto resource_system = esp::ResourceSystem::create(asset_path);

  auto params = esp::BinaryResourceParams();

  auto resource = resource_system->load<esp::BinaryResource>("bin/test.bin", params);

  REQUIRE(strcmp(static_cast<const char*>(resource->get_data()), "test") == 0);

  resource_system->unload(std::move(resource));

  resource_system->terminate();
}

TEST_CASE("Resource system - text loader", "[resource_system]")
{
  auto logger = esp::Logger::create();

  fs::path asset_path  = fs::current_path() / ".." / "tests" / "assets";
  auto resource_system = esp::ResourceSystem::create(asset_path);

  auto params = esp::TextResourceParams();

  auto resource = resource_system->load<esp::TextResource>("test.txt", params);
  std::unique_ptr<esp::TextResource> text_resource =
      std::unique_ptr<esp::TextResource>(static_cast<esp::TextResource*>(resource.release()));

  REQUIRE(strcmp(static_cast<const char*>(text_resource->get_data()), "test") == 0);
  REQUIRE(text_resource->get_num_of_lines() == 1);

  resource_system->unload(std::move(text_resource));

  resource_system->terminate();
}

class TestResource : public esp::Resource
{
 public:
  TestResource(const fs::path& path, uint64_t data_size, std::unique_ptr<void, VOID_DELETER_TYPE> data) :
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

    return std::unique_ptr<esp::Resource>(
        new TestResource(full_path, 5, std::unique_ptr<void, VOID_DELETER_TYPE>(data, VOID_DELETER)));
  }
  inline virtual void unload(std::unique_ptr<esp::Resource> resource) override { resource.reset(nullptr); }
};

TEST_CASE("Resource system - custom loader", "[resource_system]")
{
  auto logger = esp::Logger::create();

  fs::path asset_path  = fs::current_path() / ".." / "tests" / "assets";
  auto resource_system = esp::ResourceSystem::create(asset_path);

  resource_system->register_loader<TestResource>(std::unique_ptr<esp::Loader>(new TestLoader()));

  auto params   = TestResourceParams();
  auto resource = resource_system->load<TestResource>("Test", params);

  REQUIRE(strcmp(static_cast<const char*>(resource->get_data()), "test") == 0);

  resource_system->unload(std::move(resource));

  resource_system->terminate();
}