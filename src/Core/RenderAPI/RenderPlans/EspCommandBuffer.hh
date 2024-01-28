#ifndef CORE_RENDER_API_ESP_COMMAND_BUFFER_HH
#define CORE_RENDER_API_ESP_COMMAND_BUFFER_HH

#include "esppch.hh"

namespace esp
{
  class EspCommandBufferId
  {
  };

  class EspCommandBuffer
  {
    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspCommandBufferId> begin_only_once();
    static void end_only_once(std::unique_ptr<EspCommandBufferId> command_buffer_id);
  };
} // namespace esp

#endif // CORE_RENDER_API_ESP_COMMAND_BUFFER_HH
