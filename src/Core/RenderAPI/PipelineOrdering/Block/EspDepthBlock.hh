#ifndef CORE_RENDER_API_ESP_DEPTH_BLOCK_HH
#define CORE_RENDER_API_ESP_DEPTH_BLOCK_HH

#include "esppch.hh"

#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspDepthBlockFormat.hh"

// std
#include <vector>

namespace esp
{
  class EspDepthBlock
  {
    /* -------------------------- FIELDS ----------------------------------- */
   protected:
    EspDepthBlockFormat m_format;

    uint32_t m_width;
    uint32_t m_height;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspDepthBlock(EspDepthBlockFormat format, uint32_t width, uint32_t height);
    virtual ~EspDepthBlock() {}

    virtual void clear() = 0;

    virtual uint32_t get_width() const { return m_width; }
    virtual uint32_t get_height() const { return m_height; }

    /* -------------------------- METHODS STATIC --------------------------- */
   public:
    static std::unique_ptr<EspDepthBlock> build(EspDepthBlockFormat format);
  };

} // namespace esp

#endif /* CORE_RENDER_API_ESP_DEPTH_BLOCK_HH */
