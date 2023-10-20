#ifndef ESPERT_CORESYSTEMS_WINDOWEVENT_H_
#define ESPERT_CORESYSTEMS_WINDOWEVENT_H_

#include "Event.hh"

namespace Espert
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
    unsigned int _m_width;
    unsigned int _m_height;

   public:
    WindowResizedEvent(unsigned int width, unsigned int height) :
        _m_width(width), _m_height(height)
    {
    }

    inline unsigned int get_width() const { return _m_width; }
    inline unsigned int get_height() const { return _m_height; }

    EVENT_CLASS_TYPE(EventType::EventTypeWindow)
    EVENT_CLASS_SUBTYPE(WindowResized)
  };

} // namespace Espert

#endif // ESPERT_CORESYSTEMS_WINDOWEVENT_H_
