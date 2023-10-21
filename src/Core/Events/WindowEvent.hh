#ifndef ESPERT_CORE_WINDOWEVENT_H
#define ESPERT_CORE_WINDOWEVENT_H

#include "Event.hh"

namespace esp
{
  class WindowClosedEvent : public Event
  {
   public:
    WindowClosedEvent() {}

    EVENT_CLASS_TYPE(EventType::EventTypeWindow)
    EVENT_CLASS_SUBTYPE(WindowClosed)
  };

  class WindowResizedEvent : public Event
  {
   private:
    unsigned int m_width;
    unsigned int m_height;

   public:
    WindowResizedEvent(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

    inline unsigned int get_width() const { return m_width; }
    inline unsigned int get_height() const { return m_height; }

    EVENT_CLASS_TYPE(EventType::EventTypeWindow)
    EVENT_CLASS_SUBTYPE(WindowResized)
  };
} // namespace esp

#endif // ESPERT_CORE_WINDOWEVENT_H
