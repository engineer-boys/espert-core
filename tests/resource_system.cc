#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

#include "Core/Logger.hh"
#include "Core/Resources/ResourceTypes.hh"
#include "Core/Systems/ResourceSystem.hh"

TEST_CASE("Resource system", "[resource_system]")
{
  auto logger = esp::Logger::create();

  REQUIRE_FALSE(esp::ResourceSystem::is_initialized());

  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";
  auto resourceSystem = esp::ResourceSystem::init(asset_path);

  REQUIRE(esp::ResourceSystem::is_initialized);
  REQUIRE(resourceSystem->get_asset_base_path() == asset_path);

  resourceSystem->shutdown();
  REQUIRE_FALSE(esp::ResourceSystem::is_initialized());
}

TEST_CASE("Resource system - binary loader", "[resource_system]")
{
  auto logger = esp::Logger::create();

  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";
  auto resourceSystem = esp::ResourceSystem::init(asset_path);

  auto params               = esp::BinaryResourceParams();
  params.relative_directory = "bin";

  auto resource = resourceSystem->load("test.bin", esp::BinaryResource::type(), params);

  REQUIRE(strcmp(static_cast<const char*>(resource->get_data()), "test") == 0);

  resourceSystem->unload(std::move(resource));

  resourceSystem->shutdown();
}

TEST_CASE("Resource system - text loader", "[resource_system]")
{
  auto logger = esp::Logger::create();

  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";
  auto resourceSystem = esp::ResourceSystem::init(asset_path);

  auto params = esp::TextResourceParams();

  auto resource = resourceSystem->load("test.txt", esp::TextResource::type(), params);
  std::unique_ptr<esp::TextResource> textResource =
      std::unique_ptr<esp::TextResource>(static_cast<esp::TextResource*>(resource.release()));

  REQUIRE(strcmp(static_cast<const char*>(textResource->get_data()), "test") == 0);
  REQUIRE(textResource->get_num_of_lines() == 1);

  resourceSystem->unload(std::move(textResource));

  resourceSystem->shutdown();
}

class TestResource : public esp::Resource
{
 public:
  TestResource(const fs::path& path, uint64_t data_size, std::unique_ptr<void, VOID_DELETER_TYPE> data) :
      Resource(path, data_size, std::move(data))
  {
  }

  PREVENT_COPY(TestResource);

  inline static const esp::ResourceType type() { return typeid(TestResource); }
};

struct TestResourceParams : public esp::ResourceParams
{
};

class TestLoader : public esp::Loader
{
 public:
  inline virtual std::unique_ptr<esp::Resource>
  load(const std::string& name, esp::ResourceType resourceType, const esp::ResourceParams& params) override
  {
    fs::path full_path = esp::ResourceSystem::get_asset_base_path() / params.relative_directory / name;

    void* data = (void*)calloc(sizeof(char), 5);
    ESP_ASSERT(data != nullptr, "Could not allocate memory for {}.", full_path.string());

    memcpy(data, "test", 5);

    return std::unique_ptr<esp::Resource>(
        new TestResource(full_path, 5, std::unique_ptr<void, VOID_DELETER_TYPE>(data, VOID_DELETER)));
  }
  inline virtual void unload(std::unique_ptr<esp::Resource> resource) override { resource.reset(nullptr); }
  inline virtual const esp::ResourceType& get_resource_type() override { return m_resource_type; }

 private:
  const esp::ResourceType m_resource_type = typeid(TestResource);
};

TEST_CASE("Resource system - custom loader", "[resource_system]")
{
  auto logger = esp::Logger::create();

  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";
  auto resourceSystem = esp::ResourceSystem::init(asset_path);

  resourceSystem->register_loader(std::unique_ptr<esp::Loader>(new TestLoader()));

  auto params   = TestResourceParams();
  auto resource = resourceSystem->load("Test", TestResource::type(), params);

  REQUIRE(strcmp(static_cast<const char*>(resource->get_data()), "test") == 0);

  resourceSystem->unload(std::move(resource));

  resourceSystem->shutdown();
}