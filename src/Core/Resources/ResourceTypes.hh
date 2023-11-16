#ifndef ESPERT_CORE_RESOURCES_RESOURCETYPES_HH
#define ESPERT_CORE_RESOURCES_RESOURCETYPES_HH

#include "esppch.hh"

#include "Core/Resources/ResourceUtils.hh"

namespace esp
{
  class Resource
  {
   public:
    Resource(const fs::path& path, uint64_t data_size, resource_data_t data) :
        m_path(path), m_data_size(data_size), m_data(std::move(data))
    {
    }

    virtual ~Resource() {}

    PREVENT_COPY(Resource);

    inline const fs::path& get_path() const { return m_path; }
    inline const std::string get_filename() const { return m_path.filename(); }
    inline const void* get_data() { return m_data.get(); }
    inline const uint64_t get_size() { return m_data_size; }
    inline void* release() { return m_data.release(); }

   private:
    fs::path m_path;
    uint64_t m_data_size;
    resource_data_t m_data;
  };

  struct ResourceParams
  {
  };

  class BinaryResource : public Resource
  {
   public:
    BinaryResource(const fs::path& path, uint64_t data_size, resource_data_t data) :
        Resource(path, data_size, std::move(data))
    {
    }

    PREVENT_COPY(BinaryResource);
  };

  struct BinaryResourceParams : public ResourceParams
  {
  };

  class TextResource : public Resource
  {
   public:
    TextResource(const fs::path& path, uint64_t data_size, resource_data_t data, uint64_t num_of_lines) :
        Resource(path, data_size, std::move(data)), m_num_of_lines(num_of_lines)
    {
    }

    PREVENT_COPY(TextResource);

    inline const uint64_t& get_num_of_lines() { return m_num_of_lines; }

   private:
    uint64_t m_num_of_lines;
  };

  struct TextResourceParams : public ResourceParams
  {
  };

  class ImageResource : public Resource
  {
   public:
    ImageResource(const fs::path& path, resource_data_t data, uint8_t channel_count, uint32_t width, uint32_t height) :
        Resource(path, width * height * sizeof(uint8_t), std::move(data)), m_channel_count(channel_count),
        m_width(width), m_height(height)
    {
    }

    inline const uint8_t get_channel_count() const { return m_channel_count; }
    inline const uint32_t get_width() const { return m_width; }
    inline const uint32_t get_height() const { return m_height; }

    PREVENT_COPY(ImageResource);

   private:
    uint8_t m_channel_count;
    uint32_t m_width;
    uint32_t m_height;
  };

  struct ImageResourceParams : public ResourceParams
  {
    bool flip_y               = false;
    uint8_t required_channels = 4;
  };

  enum class ShaderStage : uint8_t
  {
    VERTEX                 = 0x01,
    TESSELATION_CONTROL    = 0x02,
    TESSELATION_EVALUATION = 0x04,
    GEOMETRY               = 0x08,
    FRAGMENT               = 0x10,
    COMPUTE                = 0x20
  };

  class ShaderResource : public Resource
  {
   public:
    ShaderResource(const fs::path& path, uint64_t size, resource_data_t data) : Resource(path, size, std::move(data)) {}

    PREVENT_COPY(ShaderResource);

    inline void addShaderSource(ShaderStage stage, std::unique_ptr<TextResource> shader_source)
    {
      if (m_shader_source_map.contains(stage))
      {
        ESP_CORE_ERROR("Shader source code for stage {} already added.", stage);
        return;
      }

      m_shader_source_map.insert({ stage, std::move(shader_source) });
    }

    inline std::unique_ptr<TextResource> getShaderSource(ShaderStage stage)
    {
      if (!m_shader_source_map.contains(stage))
      {
        ESP_CORE_ERROR("No shader source code for stage {}.", stage);
        return nullptr;
      }

      return std::move(m_shader_source_map.extract(stage).mapped());
    }

   private:
    std::unordered_map<ShaderStage, std::unique_ptr<TextResource>> m_shader_source_map;
  };

  struct ShaderResourceParams : public ResourceParams
  {
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_RESOURCETYPES_HH