#ifndef CORE_TIMER_HH
#define CORE_TIMER_HH

#include "esppch.hh"

namespace esp
{
  /// @brief Measures time between ticks.
  class Timer
  {
   private:
    std::chrono::high_resolution_clock::time_point m_start_time;
    float m_delta_time{ 0.f };
    float m_time_scale{ 1.f };

   public:
    /// @brief Creates instance of Timer.
    /// @return Unique pointer to created Timer.
    static std::unique_ptr<Timer> create();

    PREVENT_COPY(Timer)

    /// @brief Default destructor.
    ~Timer() = default;

    /// @brief Updates previous and current's tick time.
    void tick();
    /// @brief Resets tick time.
    void reset();

    /// @brief Sets time scale.
    /// @param time_scale Time scale.
    inline void set_time_scale(float time_scale = 1.f) { m_time_scale = time_scale; }

    /// @brief Returns delta time between ticks.
    /// @return Delta time between ticks.
    inline float get_dt() { return m_delta_time; }
    /// @brief Returns time scale.
    /// @return Time scale.
    inline float get_time_scale() { return m_time_scale; }

   private:
    Timer();
  };
} // namespace esp

#endif // CORE_TIMER_HH
