#ifndef RENDER_API_ESP_BUFFER_HH
#define RENDER_API_ESP_BUFFER_HH

#include "esppch.hh"

namespace esp
{
  /// @brief Interface for a general buffer.
  class EspBuffer
  {
   public:
    /// @brief Type of buffer.
    enum BufferType
    {
      LOCAL,
      VISIBLE
    };

    /// @brief Default constructor.
    EspBuffer()          = default;
    /// @brief Virtual destructor.
    virtual ~EspBuffer() = default;

    /// @brief Attaches buffer to be included in next operations.
    virtual void attach() = 0;
  };
} // namespace esp

#endif // RENDER_API_ESP_BUFFER_HH
