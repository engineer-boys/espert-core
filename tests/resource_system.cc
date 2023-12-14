#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <fstream>

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

    auto resource        = esp::ResourceSystem::load<esp::BinaryResource>("bin/test.bin", params);
    auto binary_resource = esp::unique_cast<esp::BinaryResource, esp::Resource>(std::move(resource));

    REQUIRE(strcmp(static_cast<const char*>(binary_resource->get_data()), "test") == 0);

    esp::ResourceSystem::unload(std::move(binary_resource));
  }
}

TEST_CASE("Resource system - text loader", "[resource_system]")
{
  auto logger         = esp::Logger::create();
  fs::path asset_path = fs::current_path() / ".." / "tests" / "assets";

  {
    auto resource_system = esp::ResourceSystem::create(asset_path);

    auto params = esp::TextResourceParams();

    auto resource      = esp::ResourceSystem::load<esp::TextResource>("test.txt", params);
    auto text_resource = esp::unique_cast<esp::TextResource, esp::Resource>(std::move(resource));

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

    auto image_params   = esp::ImageResourceParams();
    image_params.flip_y = true;
    auto resource       = esp::ResourceSystem::load<esp::ImageResource>("test.jpg", image_params);
    auto image_resource = esp::unique_cast<esp::ImageResource, esp::Resource>(std::move(resource));

    auto binary_params   = esp::BinaryResourceParams();
    resource             = esp::ResourceSystem::load<esp::BinaryResource>("test.jpg.bin", binary_params);
    auto binary_resource = esp::unique_cast<esp::BinaryResource, esp::Resource>(std::move(resource));

    REQUIRE(memcmp(static_cast<const void*>(image_resource->get_data()),
                   static_cast<const void*>(binary_resource->get_data()),
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
  TestResource(const fs::path& path, uint64_t size, std::unique_ptr<char[]> data) :
      Resource(path), m_size(size), m_data(std::move(data))
  {
  }

  PREVENT_COPY(TestResource);

  inline const uint64_t get_size() const { return m_size; }
  inline const char* get_data() const { return m_data.get(); }

 private:
  uint64_t m_size;
  std::unique_ptr<char[]> m_data;
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
        new TestResource(full_path, 5, std::move(std::unique_ptr<char[]>(static_cast<char*>(data)))));
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

    auto params        = TestResourceParams();
    auto resource      = esp::ResourceSystem::load<TestResource>("Test", params);
    auto test_resource = esp::unique_cast<TestResource, esp::Resource>(std::move(resource));

    REQUIRE(strcmp(static_cast<const char*>(test_resource->get_data()), "test") == 0);

    esp::ResourceSystem::unload(std::move(test_resource));
  }
}