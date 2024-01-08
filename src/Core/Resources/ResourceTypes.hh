#ifndef ESPERT_CORE_RESOURCES_RESOURCETYPES_HH
#define ESPERT_CORE_RESOURCES_RESOURCETYPES_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspCubemapFace.hh"
#include "Core/RenderAPI/Resources/EspShaderStage.hh"
#include "Core/Resources/ResourceUtils.hh"

namespace esp
{

  /// @brief Base resource class.
  class Resource
  {
   public:
    /// @brief Constructor that stores resource relative path.
    /// @param path Relative path of resource.
    Resource(const fs::path& path) : m_path(path) {}

    /// @brief Virtual destructor.
    virtual ~Resource() {}

    PREVENT_COPY(Resource);

    /// @brief Returns resource path.
    /// @return Reference to std::filesystem::path of resource.
    inline const fs::path& get_path() const { return m_path; }

    /// @brief Returns resource filename.
    /// @return Resource filename string.
    inline const std::string get_filename() const { return m_path.filename().string(); }

   private:
    fs::path m_path;
  };

  /// @brief Base resource parameters class.
  struct ResourceParams
  {
  };

  /// @brief Resource representing raw binary data.
  class BinaryResource : public Resource
  {
   public:
    /// @brief Constructor that sets resource path, size and data.
    /// @param path Relative path of resource.
    /// @param size Resource data size in bytes.
    /// @param data Unique pointer to void* data.
    BinaryResource(const fs::path& path, uint64_t size, resource_data_t data) :
        Resource(path), m_size(size), m_data(std::move(data))
    {
    }

    PREVENT_COPY(BinaryResource);

    /// @brief Returns raw pointer to binary data.
    /// @return Pointer to void of data.
    inline const void* get_data() const { return m_data.get(); }

    /// @brief Returns size of binary data in bytes.
    /// @return Size in bytes.
    inline const uint64_t get_size() const { return m_size; }

    /// @brief Releases underyling unique pointer and returns raw pointer to data.
    /// @return Raw pointer to void of data.
    inline void* release() { return m_data.release(); }

   private:
    resource_data_t m_data;
    uint64_t m_size;
  };

  /// @brief Parameters that might affect loading process of BinaryResource.
  struct BinaryResourceParams : public ResourceParams
  {
  };

  /// @brief Resource representing text data.
  class TextResource : public Resource
  {
   public:
    /// @brief Constructor that sets resource path, size, data and number of lines.
    /// @param path Relative path of resource.
    /// @param size Resource data size in bytes.
    /// @param data Unique pointer to array of char data.
    /// @param num_of_lines Number of lines in text file.
    TextResource(const fs::path& path, uint64_t size, std::unique_ptr<char[]> data, uint64_t num_of_lines) :
        Resource(path), m_size(size), m_data(std::move(data)), m_num_of_lines(num_of_lines)
    {
    }

    PREVENT_COPY(TextResource);

    /// @brief Returns number of lines in TextResource.
    /// @return Number of lines.
    inline const uint64_t& get_num_of_lines() { return m_num_of_lines; }

    /// @brief Returns raw pointer to char data.
    /// @return Pointer to char of data.
    inline const char* get_data() const { return m_data.get(); }

    /// @brief Returns size of char data in bytes.
    /// @return Size in bytes.
    inline const uint64_t get_size() const { return m_size; }

    /// @brief Releases underyling unique pointer and returns raw pointer to data.
    /// @return Raw pointer to char of data.
    inline char* release() { return m_data.release(); }

   private:
    std::unique_ptr<char[]> m_data;
    uint64_t m_size;
    uint64_t m_num_of_lines;
  };

  /// @brief Parameters that might affect loading process of TextResource.
  struct TextResourceParams : public ResourceParams
  {
  };

  /// @brief Resource representing image.
  class ImageResource : public Resource
  {
   public:
    /// @brief Constructor that sets image's path, size, data, channel_count, width and height.
    /// @param path Relative path of resource.
    /// @param data Unique pointer to array of pixels (uin8_t).
    /// @param size Resource data size in bytes.
    /// @param channel_count Number of image channels. (1, 2, 3 or 4)
    /// @param width Number of pixels horizontally.
    /// @param height Number of pixels vertically.
    ImageResource(const fs::path& path,
                  std::unique_ptr<uint8_t[]> data,
                  uint8_t channel_count,
                  uint32_t width,
                  uint32_t height) :
        Resource(path),
        m_data(std::move(data)), m_channel_count(channel_count), m_width(width), m_height(height)
    {
    }

    PREVENT_COPY(ImageResource);

    /// @brief Returns number of image channels.
    /// @return Number of image channels. (1, 2, 3 or 4)
    inline const uint8_t get_channel_count() const { return m_channel_count; }

    /// @brief Returns image width.
    /// @return Number of pixels horizontally.
    inline const uint32_t get_width() const { return m_width; }

    /// @brief Returns image height.
    /// @return Number of pixels vertically.
    inline const uint32_t get_height() const { return m_height; }

    /// @brief Returns image size in bytes.
    /// @return Image size in bytes.
    inline const uint64_t get_size() const { return m_channel_count * m_width * m_height; }

    /// @brief Returns pointer to raw image data.
    /// @return Pointer to image data.
    inline const uint8_t* get_data() const { return m_data.get(); }

    /// @brief Releases underyling unique pointer and returns raw pointer to data.
    /// @return Raw pointer to uint8_t of data.
    inline uint8_t* release() { return m_data.release(); }

   private:
    uint8_t m_channel_count;
    uint32_t m_width;
    uint32_t m_height;
    std::unique_ptr<uint8_t[]> m_data;
  };

  /// @brief Parameters that might affect loading process of ImageResource.
  struct ImageResourceParams : public ResourceParams
  {
    /// @brief If true image will be flipped in y axis during loading. (stb_image)
    bool flip_y = false;
    /// @brief Number of channels that are required of loaded image. (stb_image)
    uint8_t required_channels = 4;
  };

  using SpirvData    = std::vector<uint32_t>;
  using SpirvDataMap = std::unordered_map<EspShaderStage, SpirvData>;

  /// @brief Resource representing collection of SPIR-V shader source data.
  class SpirvResource : public Resource
  {
   public:
    /// @brief Constructor that sets resource path and data.
    /// @param path Relative path of resource.
    /// @param spirv_data_map Map of pairs of EspShaderStage and SpirvData.
    SpirvResource(const fs::path& path, SpirvDataMap spirv_data_map) : Resource(path)
    {
      m_spirv_data_map = std::move(spirv_data_map);
    }

    PREVENT_COPY(SpirvResource);

    /// @brief Returns SPIR-V data for specific shader stage.
    /// @param stage Stage of shader for which data will be returned.
    /// @return Vector of uint8_t of SPIR-V data.
    inline const SpirvData& get_data(EspShaderStage stage)
    {
      if (!m_spirv_data_map.contains(stage)) { ESP_CORE_ERROR("No spirv data for supplied stage."); }

      return m_spirv_data_map.at(stage);
    }

    /// @brief Runs function for every shader stage avaliable.
    /// @param func Function handle that returns void and accepts EspShaderStage and reference to SPIR-V data.
    inline void enumerate_data(std::function<void(EspShaderStage stage, const SpirvData& spirv_data)> func)
    {
      for (auto it : m_spirv_data_map)
      {
        func(it.first, it.second);
      }
    }

    /// @brief Checks if data for shader stage is avaliable.
    /// @param stage Stage of shader.
    /// @return True if data is avaliable for EsoShaderStage. False otherwise.
    inline bool is_stage_avaliable(EspShaderStage stage) { return m_spirv_data_map.contains(stage); }

   private:
    SpirvDataMap m_spirv_data_map;
  };

  /// @brief Parameters that might affect loading process of SpirvResource.
  struct SpirvResourceParams : public ResourceParams
  {
    /// @brief Flags representing which shader stages have to be avaliable in order for load to succeed.
    EspShaderStageFlags shader_stage = EspShaderStage::VERTEX | EspShaderStage::FRAGMENT;
  };

  using FaceResourceMap = std::unordered_map<EspCubemapFace, std::unique_ptr<ImageResource>>;

  /// @brief Resource representing cubemap (collection of 6 images).
  class CubemapResource : public Resource
  {
   public:
    /// @brief Constructor that sets resource path and map of cubemap faces.
    /// @param path Relative path of resource.
    /// @param face_resource_map Map of cubemap faces.
    CubemapResource(const fs::path& path, FaceResourceMap face_resource_map) :
        Resource(path), m_face_resource_map(std::move(face_resource_map))
    {
      m_channel_count = get_face(EspCubemapFace::RIGHT).get_channel_count();
    }

    PREVENT_COPY(CubemapResource);

    /// @brief return number of cubemap's images' channels
    /// @return number of channels
    inline const uint8_t get_channel_count() const { return m_channel_count; }

    /// @brief Returns ImageResource for specific face.
    /// @param face Type of face for which data will be returned.
    /// @return Reference to ImageResoruce of specific face.
    inline const ImageResource& get_face(EspCubemapFace face) { return *(m_face_resource_map.at(face)); }

   private:
    uint8_t m_channel_count;
    FaceResourceMap m_face_resource_map;
  };

  /// @brief Parameters that might affect loading process of CubemapResource.
  struct CubemapResourceParams : public ImageResourceParams
  {
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_RESOURCETYPES_HH