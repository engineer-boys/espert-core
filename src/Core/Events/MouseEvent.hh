#ifndef ESPERT_CORE_MOUSEEVENT_H_
#define ESPERT_CORE_MOUSEEVENT_H_

#include "Event.hh"

namespace esp
{
    class MouseMovedEvent : public Event
    {
    private:
        float _m_x;
        float _m_y;

    public:
        MouseMovedEvent(float x, float y) : _m_x(x), _m_y(y) {}

        inline float get_x() const { return _m_x; }
        inline float get_y() const { return _m_y; }

        EVENT_CLASS_TYPE(EventType::EventTypeInput | EventType::EventTypeMouse)
        EVENT_CLASS_SUBTYPE(MouseMoved)
    };

    class MouseScrolledEvent : public Event
    {
    private:
        float _m_offset_x;
        float _m_offset_y;

    public:
        MouseScrolledEvent(float o_x, float o_y) : _m_offset_x(o_x), _m_offset_y(o_y) {}

        inline float get_offset_x() const { return _m_offset_x; }
        inline float get_offset_y() const { return _m_offset_y; }

        EVENT_CLASS_TYPE(EventType::EventTypeInput | EventType::EventTypeMouse)
        EVENT_CLASS_SUBTYPE(MouseScrolled)
    };

    class MouseButtonEvent : public Event
    {
    protected:
        int _m_button_code;

        MouseButtonEvent(int code) : _m_button_code(code) {}

    public:
        inline int get_button_code() const { return _m_button_code; }

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
} // namespace Espert

#endif  // ESPERT_CORE_MOUSEEVENT_H_
