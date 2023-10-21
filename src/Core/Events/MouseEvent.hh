#ifndef ESPERT_CORE_MOUSEEVENT_H
#define ESPERT_CORE_MOUSEEVENT_H

#include "Event.hh"

namespace esp
{
  class MouseMovedEvent : public Event
  {
   private:
    float m_x;
    float m_y;

   public:
    MouseMovedEvent(float x, float y) : m_x(x), m_y(y) {}

    inline float get_x() const { return m_x; }
    inline float get_y() const { return m_y; }

    EVENT_CLASS_TYPE(EventType::EventTypeInput | EventType::EventTypeMouse)
    EVENT_CLASS_SUBTYPE(MouseMoved)
  };

  class MouseScrolledEvent : public Event
  {
   private:
    float m_offset_x;
    float m_offset_y;

   public:
    MouseScrolledEvent(float o_x, float o_y) : m_offset_x(o_x), m_offset_y(o_y) {}

    inline float get_offset_x() const { return m_offset_x; }
    inline float get_offset_y() const { return m_offset_y; }

    EVENT_CLASS_TYPE(EventType::EventTypeInput | EventType::EventTypeMouse)
    EVENT_CLASS_SUBTYPE(MouseScrolled)
  };

  class MouseButtonEvent : public Event
  {
   protected:
    int m_button_code;

    MouseButtonEvent(int code) : m_button_code(code) {}

   public:
    inline int get_button_code() const { return m_button_code; }

    EVENT_CLASS_TYPE(EventType::EventTypeInput | EventType::EventTypeMouse)
  };

  class MouseButtonPressedEvent : public MouseButtonEvent
  {
   public:
    MouseButtonPressedEvent(int code) : MouseButtonEvent(code) {}

    EVENT_CLASS_SUBTYPE(MouseButtonPressed)
  };

  class MouseButtonReleasedEvent : public MouseButtonEvent
  {
   public:
    MouseButtonReleasedEvent(int code) : MouseButtonEvent(code) {}

    EVENT_CLASS_SUBTYPE(MouseButtonReleased)
  };
} // namespace esp

#endif // ESPERT_CORE_MOUSEEVENT_H
