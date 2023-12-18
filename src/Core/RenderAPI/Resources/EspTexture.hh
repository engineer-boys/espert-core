#ifndef RENDER_API_ESP_TEXTURE_HH
#define RENDER_API_ESP_TEXTURE_HH

#include "esppch.hh"

#include "Core/Resources/ResourceTypes.hh"

namespace esp
{
  using EspTextureTypeFlags = uint8_t;
  enum class EspTextureType : uint8_t
  {
    ALBEDO    = 0x01,
    NORMAL    = 0x02,
    METALLIC  = 0x04,
    ROUGHNESS = 0x08,
    AO        = 0x10,
    ENUM_END  = 0x20
  };

  inline EspTextureTypeFlags operator|(const EspTextureType a, const EspTextureType b)
  {
    return static_cast<EspTextureTypeFlags>(a) | static_cast<EspTextureTypeFlags>(b);
  }

  inline EspTextureTypeFlags operator|(const EspTextureTypeFlags a, const EspTextureType b)
  {
    return a | static_cast<EspTextureTypeFlags>(b);
  }

  inline EspTextureTypeFlags operator&(const EspTextureTypeFlags a, const EspTextureType b)
  {
    return a & static_cast<EspTextureTypeFlags>(b);
  }

  inline EspTextureType operator++(EspTextureType& a)
  {
    if (a == EspTextureType::ENUM_END) return a;
    auto copy = a;
    a         = static_cast<EspTextureType>(static_cast<uint8_t>(a) << 1);
    return copy;
  }

  inline EspTextureType operator++(EspTextureType& a, int dummy)
  {
    if (a == EspTextureType::ENUM_END) return a;
    a = static_cast<EspTextureType>(static_cast<uint8_t>(a) << 1);
    return a;
  }

  inline EspTextureType operator--(EspTextureType& a)
  {
    if (a == EspTextureType::ALBEDO) return a;
    auto copy = a;
    a         = static_cast<EspTextureType>(static_cast<uint8_t>(a) >> 1);
    return copy;
  }

  inline EspTextureType operator--(EspTextureType& a, int dummy)
  {
    if (a == EspTextureType::ALBEDO) return a;
    a = static_cast<EspTextureType>(static_cast<uint8_t>(a) >> 1);
    return a;
  }

  class EspTexture
  {
   public:
    PREVENT_COPY(EspTexture);

    static std::shared_ptr<EspTexture> create(const std::string& name,
                                              std::unique_ptr<ImageResource> image,
                                              EspTextureType type = EspTextureType::ALBEDO,
                                              bool mipmapping     = false);

    inline const std::string& get_name() const { return m_name; }
    inline const uint64_t get_size() const { return m_channel_count * m_width * m_height; }
    inline const uint8_t get_channel_count() const { return m_channel_count; }
    inline const uint32_t get_width() const { return m_width; }
    inline const uint32_t get_height() const { return m_height; }
    inline const uint32_t get_mip_levels() const { return m_mip_levels; }
    inline const bool has_transparency() const { return m_has_transparency; }
    inline const EspTextureType get_type() const { return m_type; }

   protected:
    EspTexture(const std::string& name,
               const uint8_t* pixels,
               uint8_t channel_count,
               uint32_t width,
               uint32_t height,
               EspTextureType type = EspTextureType::ALBEDO);
    EspTexture(uint32_t width, uint32_t height);

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

#endif // RENDER_API_ESP_TEXTURE_HH
