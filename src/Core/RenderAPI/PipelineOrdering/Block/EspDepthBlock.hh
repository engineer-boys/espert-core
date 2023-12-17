#ifndef CORE_RENDER_API_ESP_DEPTH_BLOCK_HH
#define CORE_RENDER_API_ESP_DEPTH_BLOCK_HH

#include "esppch.hh"

// std
#include <vector>

namespace esp
{
  class EspDepthBlock
  {
    /* -------------------------- FIELDS ----------------------------------- */
   protected:
    uint32_t m_width;
    uint32_t m_height;

    glm::vec3 m_clear_color;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspDepthBlock(uint32_t width, uint32_t height, glm::vec3 clear_color);
    virtual ~EspDepthBlock() {}

    virtual uint32_t get_width() const { return m_width; }
    virtual uint32_t get_height() const { return m_height; }
    virtual glm::vec3 get_clear_color() const { return m_clear_color; }

    /* -------------------------- METHODS STATIC --------------------------- */
   public:
    static std::unique_ptr<EspDepthBlock> build(uint32_t width, uint32_t height, glm::vec3 clear_color);
  };

} // namespace esp

#endif /* CORE_RENDER_API_ESP_DEPTH_BLOCK_HH */
