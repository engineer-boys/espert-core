#ifndef __ENGINE__KEYEVENT_HH__
#define __ENGINE__KEYEVENT_HH__

#include "Event.hh"

namespace Esper
{
    class KeyEvent : public Event
    {
    protected:
        int _m_key_code;

        KeyEvent(int code) : _m_key_code(code) {}

    public:
        inline int get_code() const { return _m_key_code; }

        EVENT_CLASS_TYPE(EventType::EventTypeInput | EventType::EventTypeKeyboard)
    };

    class KeyPressedEvent : public KeyEvent
    {
    private:
        bool _m_is_repeated;

    public:
        KeyPressedEvent(int code, bool is_repeated) : KeyEvent(code), _m_is_repeated(is_repeated) {}

        inline bool is_repeated() const { return _m_is_repeated; }

        EVENT_CLASS_SUBTYPE(KeyPressed)
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(int code) : KeyEvent(code) {}

        EVENT_CLASS_SUBTYPE(KeyReleased)
    };
} // namespace Esper

#endif /* __ENGINE__KEYEVENT_HH__ */
