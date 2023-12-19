#ifndef CORE_RENDER_API_ESP_DEPTH_BLOCK_HH
#define CORE_RENDER_API_ESP_DEPTH_BLOCK_HH

#include "esppch.hh"

#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspDepthBlockFormat.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspSampleCountFlag.hh"

// std
#include <vector>

namespace esp
{
  class EspDepthBlock
  {
    /* -------------------------- FIELDS ----------------------------------- */
   protected:
    EspSampleCountFlag m_sample_count_flag;
    EspDepthBlockFormat m_format;

    uint32_t m_width;
    uint32_t m_height;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspDepthBlock(EspDepthBlockFormat format, EspSampleCountFlag sample_count_flag, uint32_t width, uint32_t height);
    virtual ~EspDepthBlock() {}

    virtual void clear() = 0;

    virtual inline uint32_t get_width() const { return m_width; }
    virtual inline uint32_t get_height() const { return m_height; }
    virtual inline EspSampleCountFlag get_sample_count_flag() { return m_sample_count_flag; }

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspDepthBlock> build(EspDepthBlockFormat format, EspSampleCountFlag sample_count_flag);
  };

} // namespace esp

#endif /* CORE_RENDER_API_ESP_DEPTH_BLOCK_HH */
