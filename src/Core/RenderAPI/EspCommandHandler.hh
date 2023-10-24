#ifndef RENDER_API_ESP_COMMAND_HANDLER_H
#define RENDER_API_ESP_COMMAND_HANDLER_H

#include "esppch.hh"

namespace esp
{
  class EspCommandHandler
  {
   private:
    static bool s_is_exist;

   public:
    static std::unique_ptr<EspCommandHandler> create_and_init();

    EspCommandHandler()          = default;
    virtual ~EspCommandHandler() = default;

    EspCommandHandler(const EspCommandHandler& other)            = delete;
    EspCommandHandler& operator=(const EspCommandHandler& other) = delete;

    virtual void init() = 0;
    // virtual void bind_pipeline(EspPipeline);
  };
} // namespace esp

#endif // RENDER_API_ESP_COMMAND_HANDLER_H
