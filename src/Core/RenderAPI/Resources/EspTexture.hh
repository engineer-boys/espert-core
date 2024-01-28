#ifndef RENDER_API_ESP_TEXTURE_HH
#define RENDER_API_ESP_TEXTURE_HH

#include "esppch.hh"

#include "Core/RenderAPI/RenderPlans/Block/Types/EspSampleCountFlag.hh"
#include "Core/RenderAPI/Resources/Format/EspTextureFormat.hh"
#include "Core/Resources/ResourceTypes.hh"

namespace esp
{
  using EspTextureTypeFlags = uint8_t;
  /// @brief Describes the type of texture. (currently PBR only)
  enum class EspTextureType : uint8_t
  {
    ALBEDO    = 0x01,
    NORMAL    = 0x02,
    METALLIC  = 0x04,
    ROUGHNESS = 0x08,
    AO        = 0x10,
    ENUM_END  = 0x20
  };

  /// @brief Connects two EspTextureTypes to form EspTextureTypeFlags.
  /// @param a First EspTextureType.
  /// @param b Second EspTextureType.
  /// @return EspTextureTypeFlags with bits set for both types.
  inline EspTextureTypeFlags operator|(const EspTextureType a, const EspTextureType b)
  {
    return static_cast<EspTextureTypeFlags>(a) | static_cast<EspTextureTypeFlags>(b);
  }

  /// @brief Adds EspTextureType to EspTextureTypeFlags.
  /// @param a EspTextureTypeFlags to set the type on.
  /// @param b EspTextureTypeFlag to set in EspTextureTypeFlags.
  /// @return EspTextureTypeFlags with new type set.
  inline EspTextureTypeFlags operator|(const EspTextureTypeFlags a, const EspTextureType b)
  {
    return a | static_cast<EspTextureTypeFlags>(b);
  }

  /// @brief Unsets EspTextureType from EspTextureTypeFlags.
  /// @param a EspTextureTypeFlags to unset EspTextureType from.
  /// @param b EspTextureType that will be unset in EspTextureTypeFlags.
  /// @return EspTextureTypeFlags with type unset.
  inline EspTextureTypeFlags operator&(const EspTextureTypeFlags a, const EspTextureType b)
  {
    return a & static_cast<EspTextureTypeFlags>(b);
  }

  /// @brief Sets texture type to next type. (useful for iterating enum)
  /// @param a Reference to texture type.
  /// @return Texture type before 'incrementation'.
  inline EspTextureType operator++(EspTextureType& a)
  {
    if (a == EspTextureType::ENUM_END) return a;
    auto copy = a;
    a         = static_cast<EspTextureType>(static_cast<uint8_t>(a) << 1);
    return copy;
  }

  /// @brief Sets texture type to next type type. (useful for iterating enum)
  /// @param a Reference to texture type.
  /// @param dummy Dummy parameter needed to differentiate incrementation operators.
  /// @return Texture type after 'incrementation'.
  inline EspTextureType operator++(EspTextureType& a, int dummy)
  {
    if (a == EspTextureType::ENUM_END) return a;
    a = static_cast<EspTextureType>(static_cast<uint8_t>(a) << 1);
    return a;
  }

  /// @brief Sets texture type to previous type type. (useful for iterating enum)
  /// @param a Reference to texture type.
  /// @return Texture type before 'decrementation'
  inline EspTextureType operator--(EspTextureType& a)
  {
    if (a == EspTextureType::ALBEDO) return a;
    auto copy = a;
    a         = static_cast<EspTextureType>(static_cast<uint8_t>(a) >> 1);
    return copy;
  }

  /// @brief Sets texture type to previous type type. (useful for iterating enum)
  /// @param a Reference to texture type.
  /// @param dummy Dummy parameter needed to differentiate decrementation operators.
  /// @return Texture type before 'decrementation'.
  inline EspTextureType operator--(EspTextureType& a, int dummy)
  {
    if (a == EspTextureType::ALBEDO) return a;
    a = static_cast<EspTextureType>(static_cast<uint8_t>(a) >> 1);
    return a;
  }

  /// @brief Converts texture type to human readable string.
  /// @param face texture type.
  /// @return Human readable string describing texture type.
  inline aiTextureType esp_texture_type_to_assimp(EspTextureType type)
  {
    switch (type)
    {
    case EspTextureType::ALBEDO:
      return aiTextureType_DIFFUSE;

    case EspTextureType::NORMAL:
      return aiTextureType_NORMALS;

    case EspTextureType::METALLIC:
      return aiTextureType_METALNESS;

    case EspTextureType::ROUGHNESS:
      return aiTextureType_DIFFUSE_ROUGHNESS;

    case EspTextureType::AO:
      return aiTextureType_AMBIENT_OCCLUSION;

    default:
      return aiTextureType_NONE;
    }
  }

  struct EspRawTextureParams
  {
    EspTextureFormat format;
    EspSampleCountFlag sample_count = EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT;

    uint32_t width;
    uint32_t height;
    uint32_t mip_levels = 1;

    bool as_cubemap = false;
  };

  /// @brief Texture stored on the GPU.
  class EspTexture
  {
   public:
    PREVENT_COPY(EspTexture);

    /// @brief Virtual destructor.
    virtual ~EspTexture() {}

    /// @brief Creates texture with all necessary components.
    /// @param name Texture name.
    /// @param image Image resource containing pixel data.
    /// @param type Type of texture.
    /// @param mipmapping Bool indicating if texture uses mipmaps.
    /// @param format Format of textures pixel data.
    /// @return Shared pointer to instance of texture.
    static std::shared_ptr<EspTexture> create(const std::string& name,
                                              std::unique_ptr<ImageResource> image,
                                              EspTextureType type     = EspTextureType::ALBEDO,
                                              bool mipmapping         = false,
                                              EspTextureFormat format = EspTextureFormat::ESP_FORMAT_R8G8B8A8_SRGB);

    /// @brief Creates cubemap texture with all necessary components.
    /// @param name Cubemap name.
    /// @param cubemap_resource Cubemap resource containing pixel data for all 6 faces.
    /// @param format Format of cubemap pixel data.
    /// @return Shared pointer to instance of cubemap.
    static std::shared_ptr<EspTexture> create_cubemap(const std::string& name,
                                                      std::unique_ptr<CubemapResource> cubemap_resource,
                                                      EspTextureFormat format);

    static std::shared_ptr<EspTexture> create_raw_texture(EspRawTextureParams params);

    /// @brief Returns texture name.
    /// @return Texture name.
    inline const std::string& get_name() const { return m_name; }
    /// @brief Returns texture size.
    /// @return Texture size.
    inline const uint64_t get_size() const { return m_channel_count * m_width * m_height; }
    /// @brief Returns texture channel count.
    /// @return Texture channel count.
    inline const uint8_t get_channel_count() const { return m_channel_count; }
    /// @brief Returns texture width.
    /// @return Texture width.
    inline const uint32_t get_width() const { return m_width; }
    /// @brief Returns texture height.
    /// @return Texture height.
    inline const uint32_t get_height() const { return m_height; }
    /// @brief Returns number of texture mip levels.
    /// @return Number of texture mip levels.
    inline const uint32_t get_mip_levels() const { return m_mip_levels; }
    /// @brief Indicator if texture contains transparent pixels.
    /// @return True if texture contains transparent pixels. False otherwise.
    inline const bool has_transparency() const { return m_has_transparency; }
    /// @brief Returns texture type.
    /// @return Texture type.
    inline const EspTextureType get_type() const { return m_type; }
    /// @brief Equals operator to compare if two textures have the same parameters.
    /// @param other The other texture to compare to.
    /// @return True if textures habe the same parameters. False otherwise.
    inline bool operator==(const EspTexture& other)
    {
      return m_name == other.get_name() && m_channel_count == other.get_channel_count() &&
          m_width == other.get_width() && m_height == other.get_height() && m_mip_levels == other.get_mip_levels() &&
          m_has_transparency == other.has_transparency() && m_type == other.get_type();
    }

   protected:
    EspTexture(const std::string& name,
               const uint8_t* pixels,
               uint8_t channel_count,
               uint32_t width,
               uint32_t height,
               EspTextureType type = EspTextureType::ALBEDO);
    EspTexture(uint32_t width, uint32_t height, uint32_t mip_levels = 1);
    EspTexture(const std::string& name, uint8_t channel_count, uint32_t width, uint32_t height);

    std::string m_name;
    uint8_t m_channel_count;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_mip_levels;
    bool m_has_transparency;
    EspTextureType m_type;

   private:
    void calculate_mip_levels();
    void check_for_transparency(const uint8_t* pixels);
  };
} // namespace esp

/// @brief Hashes EspTexture.
template<> struct std::hash<esp::EspTexture>
{
  /// @brief Hashes EspTexture.
  /// @param k EspTexture to be hashed.
  /// @return EspTexture hash.
  std::size_t operator()(const esp::EspTexture& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.get_name());
    esp::hash_combine(seed, k.get_channel_count());
    esp::hash_combine(seed, k.get_height());
    esp::hash_combine(seed, k.get_width());
    esp::hash_combine(seed, k.get_mip_levels());
    esp::hash_combine(seed, k.has_transparency());
    esp::hash_combine(seed, k.get_type());
    return seed;
  }
};

/// @brief Hashes shared pointer to EspTexture.
template<> struct std::hash<std::shared_ptr<esp::EspTexture>>
{
  /// @brief Hashes shared pointer to EspTexture.
  /// @param k Shared pointer to EspTexture to be hashed.
  /// @return Shared pointer to EspTexture hash.
  std::size_t operator()(const std::shared_ptr<esp::EspTexture>& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, *k);
    return seed;
  }
};

/// @brief Hashes vector of shared pointers to EspTexture.
template<> struct std::hash<std::vector<std::shared_ptr<esp::EspTexture>>>
{
  /// @brief Hashes vector of shared pointers to EspTexture.
  /// @param k Sector of shared pointers to EspTexture to be hashed.
  /// @return Sector of shared pointers to EspTexture hash.
  std::size_t operator()(const std::vector<std::shared_ptr<esp::EspTexture>>& k) const
  {
    std::size_t seed = 0;
    for (const auto& t : k)
    {
      esp::hash_combine(seed, t);
    }
    return seed;
  }
};

#endif // RENDER_API_ESP_TEXTURE_HH
