#ifndef RENDER_API_ESP_BUFFER_HH
#define RENDER_API_ESP_BUFFER_HH

#include "esppch.hh"

namespace esp
{
  class EspBuffer
  {
   public:
    EspBuffer()          = default;
    virtual ~EspBuffer() = default;

    virtual void attach() = 0;
  };
} // namespace esp

#endif // RENDER_API_ESP_BUFFER_HH
