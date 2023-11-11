#include "Timer.hh"

namespace esp
{
  std::unique_ptr<Timer> Timer::create() { return std::unique_ptr<Timer>(new Timer()); }

  void Timer::tick()
  {
    auto current_time = std::chrono::high_resolution_clock::now();
    m_delta_time      = std::chrono::duration<float, std::chrono::seconds::period>(current_time - m_start_time).count();
  }

  void Timer::reset() { m_start_time = std::chrono::high_resolution_clock::now(); }

  Timer::Timer() { reset(); }
} // namespace esp