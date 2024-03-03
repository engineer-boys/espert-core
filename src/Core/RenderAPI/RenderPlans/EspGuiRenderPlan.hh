#ifndef ESPERT_SANDBOX_ESPGUIRENDERPLAN_HH
#define ESPERT_SANDBOX_ESPGUIRENDERPLAN_HH

#include "esppch.hh"

namespace esp
{
  struct EspRenderArea
  {
    int m_x;
    int m_y;
    uint32_t m_width;
    uint32_t m_height;

    inline int get_render_x() { return std::max(m_x, 0); }
    inline int get_render_y() { return std::max(m_y, 0); }
    inline uint32_t get_render_width(uint32_t window_width)
    {
      uint32_t render_width = m_width;
      if (m_x + m_width > window_width) { render_width = window_width - m_x; }
      if (m_x < 0) { render_width += m_x; }

      return render_width;
    }

    inline uint32_t get_render_height(uint32_t window_height)
    {
      uint32_t render_height = m_height;
      if (m_y + m_height > window_height) { render_height = window_height - m_y; }
      if (m_y < 0) { render_height += m_y; }

      return render_height;
    }
  };

  class EspGuiRenderPlan
  {
   public:
    EspGuiRenderPlan()          = default;
    virtual ~EspGuiRenderPlan() = default;

    virtual void build() = 0;

    virtual void begin_plan(EspRenderArea render_area) = 0;
    virtual void end_plan()                            = 0;

    static std::unique_ptr<EspGuiRenderPlan> create();
  };
} // namespace esp

#endif // ESPERT_SANDBOX_ESPGUIRENDERPLAN_HH
