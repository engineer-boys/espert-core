#ifndef __ENGINE__EVENT_HH__
#define __ENGINE__EVENT_HH__

#include <functional>

#define SET_BIT(b) (1 << (b))

namespace Esper
{
    class Event
    {
    public:
        enum class EventSubtype
        {
            None,
            /* for window class */
            WindowClosed,
            WindowResized,
            /* for input */
            KeyPressed,
            KeyReleased,
            MouseButtonPressed,
            MouseButtonReleased,
            MouseMoved,
            MouseScrolled
        };

        enum EventType
        {
            None = 0,
            EventTypeWindow = SET_BIT(0),
            EventTypeInput = SET_BIT(1),
            EventTypeKeyboard = SET_BIT(2),
            EventTypeMouse = SET_BIT(3),
        };

        template <typename T>
        using EventHandler = std::function<bool(T &)>;

    public:
        bool handled = false;

    public:
        virtual EventSubtype get_subtype() const = 0;
        virtual int get_type() const = 0;

        inline bool is_type(EventType type)
        {
            return get_type() & type;
        }

        template <typename T>
        static bool try_hanlder(Event &event, EventHandler<T> handler)
        {
            if (event.get_subtype() == T::get_class_subtype())
            {
                event.handled = handler(*(T *)&event);
                return true;
            }

            return false;
        }
    };

} // namespace Esper

#define EVENT_CLASS_SUBTYPE(subtype)                                                                      \
    static Esper::Event::EventSubtype get_class_subtype() { return Esper::Event::EventSubtype::subtype; } \
    virtual Esper::Event::EventSubtype get_subtype() const override { return get_class_subtype(); }

#define EVENT_CLASS_TYPE(type) \
    virtual int get_type() const override { return type; }

#endif /* __ENGINE__EVENT_HH__ */
