#ifndef RENDER_API_ESP_JOB_H
#define RENDER_API_ESP_JOB_H

#include "esppch.hh"

namespace esp
{
  class EspJobs
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspJobs()          = default;
    virtual ~EspJobs() = default;

    EspJobs(const EspJobs& other)            = delete;
    EspJobs& operator=(const EspJobs& other) = delete;

    virtual void init()          = 0;
    virtual void terminate()     = 0;
    virtual void done_all_jobs() = 0;

    static void draw(uint32_t vertex_count);
    static void draw(uint32_t vertex_count, uint32_t instance_count);
    static void draw_indexed(uint32_t index_count);
    static void draw_indexed(uint32_t index_count, uint32_t instance_count);

    /* -------------------------- METHODS STATIC --------------------------- */
   public:
    static std::unique_ptr<EspJobs> build();
  };
} // namespace esp

#endif // RENDER_API_ESP_JOB_H
