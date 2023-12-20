#ifndef CORE_TIMER_HH
#define CORE_TIMER_HH

#include "esppch.hh"

namespace esp
{
  class Timer
  {
   private:
    std::chrono::high_resolution_clock::time_point m_start_time;
    float m_delta_time{ 0.f };
    float m_time_scale{ 1.f };

   public:
    static std::unique_ptr<Timer> create();

    PREVENT_COPY(Timer)

    ~Timer() = default;

    void tick();
    void reset();

    inline void set_time_scale(float time_scale = 1.f) { m_time_scale = time_scale; }

    inline float get_dt() { return m_delta_time; }
    inline float get_time_scale() { return m_time_scale; }

   private:
    Timer();
  };
} // namespace esp

#endif // CORE_TIMER_HH
