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

    virtual void init()      = 0;
    virtual void terminate() = 0;

    static void draw(uint32_t vertex_count);
    static void draw(uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(uint32_t index_count);
    static void draw_indexed(uint32_t index_count, uint32_t instance_count);
  };
} // namespace esp

#endif // RENDER_API_ESP_COMMAND_HANDLER_H
