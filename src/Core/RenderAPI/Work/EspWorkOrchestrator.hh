#ifndef RENDER_API_ESP_WORK_ORCHESTRATION_HH
#define RENDER_API_ESP_WORK_ORCHESTRATION_HH

#include "Core/EspWindow.hh"
#include "Core/Events/WindowEvent.hh"

#include "esppch.hh"

namespace esp
{
  class EspWorkOrchestrator
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspWorkOrchestrator()          = default;
    virtual ~EspWorkOrchestrator() = default;

    EspWorkOrchestrator(const EspWorkOrchestrator& other)            = delete;
    EspWorkOrchestrator& operator=(const EspWorkOrchestrator& other) = delete;

    virtual void init()      = 0;
    virtual void terminate() = 0;

    virtual void begin_frame() = 0;
    virtual void end_frame()   = 0;

    static std::pair<uint32_t, uint32_t> get_swap_chain_extent();
    static float get_swap_chain_extent_aspect_ratio();

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspWorkOrchestrator> build();
  };
} // namespace esp

#endif // RENDER_API_ESP_WORK_ORCHESTRATION_HH
