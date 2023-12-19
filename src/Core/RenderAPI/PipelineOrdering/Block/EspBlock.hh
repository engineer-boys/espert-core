#ifndef CORE_RENDER_API_ESP_BLOCK_HH
#define CORE_RENDER_API_ESP_BLOCK_HH

#include "esppch.hh"

#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspBlockFormat.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspSampleCountFlag.hh"

// std
#include <vector>

namespace esp
{
  class EspBlock
  {
    /* -------------------------- FIELDS ----------------------------------- */
   protected:
    EspSampleCountFlag m_sample_count_flag;
    EspBlockFormat m_format;

    uint32_t m_width;
    uint32_t m_height;

    glm::vec3 m_clear_color;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspBlock(EspBlockFormat format,
             EspSampleCountFlag sample_count_flag,
             uint32_t width,
             uint32_t height,
             glm::vec3 clear_color);
    virtual ~EspBlock() {}

    virtual inline uint32_t get_width() const { return m_width; }
    virtual inline uint32_t get_height() const { return m_height; }
    virtual inline glm::vec3 get_clear_color() const { return m_clear_color; }
    virtual inline EspSampleCountFlag get_sample_count_flag() { return m_sample_count_flag; }

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::shared_ptr<EspBlock> build(EspBlockFormat format,
                                           EspSampleCountFlag sample_count_flag,
                                           glm::vec3 clear_color);
  };

} // namespace esp

#endif /* CORE_RENDER_API_ESP_BLOCK_HH */
