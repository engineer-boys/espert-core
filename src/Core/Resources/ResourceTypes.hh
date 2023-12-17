#ifndef ESPERT_CORE_RESOURCES_RESOURCETYPES_HH
#define ESPERT_CORE_RESOURCES_RESOURCETYPES_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShaderStage.hh"
#include "Core/Resources/ResourceUtils.hh"

namespace esp
{
  class Resource
  {
   public:
    Resource(const fs::path& path) : m_path(path) {}

    virtual ~Resource() {}

    PREVENT_COPY(Resource);

    inline const fs::path& get_path() const { return m_path; }
    inline const std::string get_filename() const { return m_path.filename(); }

   private:
    fs::path m_path;
  };

  struct ResourceParams
  {
  };

  class BinaryResource : public Resource
  {
   public:
    BinaryResource(const fs::path& path, uint64_t size, resource_data_t data) :
        Resource(path), m_size(size), m_data(std::move(data))
    {
    }

    PREVENT_COPY(BinaryResource);

    inline const void* get_data() const { return m_data.get(); }
    inline const uint64_t get_size() const { return m_size; }
    inline void* release() { return m_data.release(); }

   private:
    resource_data_t m_data;
    uint64_t m_size;
  };

  struct BinaryResourceParams : public ResourceParams
  {
  };

  class TextResource : public Resource
  {
   public:
    TextResource(const fs::path& path, uint64_t size, std::unique_ptr<char[]> data, uint64_t num_of_lines) :
        Resource(path), m_size(size), m_data(std::move(data)), m_num_of_lines(num_of_lines)
    {
    }

    PREVENT_COPY(TextResource);

    inline const uint64_t& get_num_of_lines() { return m_num_of_lines; }
    inline const char* get_data() const { return m_data.get(); }
    inline const uint64_t get_size() const { return m_size; }
    inline char* release() { return m_data.release(); }

   private:
    std::unique_ptr<char[]> m_data;
    uint64_t m_size;
    uint64_t m_num_of_lines;
  };

  struct TextResourceParams : public ResourceParams
  {
  };

  class ImageResource : public Resource
  {
   public:
    ImageResource(const fs::path& path,
                  std::unique_ptr<uint8_t[]> data,
                  uint8_t channel_count,
                  uint32_t width,
                  uint32_t height) :
        Resource(path),
        m_data(std::move(data)), m_channel_count(channel_count), m_width(width), m_height(height)
    {
    }

    inline const uint8_t get_channel_count() const { return m_channel_count; }
    inline const uint32_t get_width() const { return m_width; }
    inline const uint32_t get_height() const { return m_height; }
    inline const uint64_t get_size() const { return m_channel_count * m_width * m_height; }
    inline const uint8_t* get_data() const { return m_data.get(); }
    inline uint8_t* release() { return m_data.release(); }

    PREVENT_COPY(ImageResource);

   private:
    uint8_t m_channel_count;
    uint32_t m_width;
    uint32_t m_height;
    std::unique_ptr<uint8_t[]> m_data;
  };

  struct ImageResourceParams : public ResourceParams
  {
    bool flip_y               = false;
    uint8_t required_channels = 4;
  };

  using SpirvData    = std::vector<uint32_t>;
  using SpirvDataMap = std::unordered_map<EspShaderStage, SpirvData>;

  class SpirvResource : public Resource
  {
   public:
    SpirvResource(const fs::path& path, SpirvDataMap spirv_data_map) : Resource(path)
    {
      m_spirv_data_map = std::move(spirv_data_map);
    }

    PREVENT_COPY(SpirvResource);

    inline const SpirvData& get_data(EspShaderStage stage)
    {
      if (!m_spirv_data_map.contains(stage)) { ESP_CORE_ERROR("No spirv data for supplied stage."); }

      return m_spirv_data_map.at(stage);
    }

    inline void enumerate_data(std::function<void(EspShaderStage stage, const SpirvData& spirv_data)> func)
    {
      for (auto it : m_spirv_data_map)
      {
        func(it.first, it.second);
      }
    }

    inline bool is_stage_avaliable(EspShaderStage stage) { return m_spirv_data_map.contains(stage); }

   private:
    SpirvDataMap m_spirv_data_map;
  };

  struct SpirvResourceParams : public ResourceParams
  {
    EspShaderStageFlags shader_stage = EspShaderStage::VERTEX | EspShaderStage::FRAGMENT;
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_RESOURCETYPES_HH