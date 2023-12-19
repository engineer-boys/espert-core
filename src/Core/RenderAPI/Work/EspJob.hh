#ifndef RENDER_API_ESP_JOB_H
#define RENDER_API_ESP_JOB_H

#include "esppch.hh"

namespace esp
{
  class EspJob
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspJob()          = default;
    virtual ~EspJob() = default;

    EspJob(const EspJob& other)            = delete;
    EspJob& operator=(const EspJob& other) = delete;

    virtual void init()          = 0;
    virtual void terminate()     = 0;
    virtual void done_all_jobs() = 0;

    static void draw(uint32_t vertex_count);
    static void draw(uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(uint32_t index_count);
    static void draw_indexed(uint32_t index_count, uint32_t instance_count);

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspJob> build();
  };
} // namespace esp

#endif // RENDER_API_ESP_JOB_H
