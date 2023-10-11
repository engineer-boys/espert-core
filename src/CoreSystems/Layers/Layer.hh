#ifndef __ENGINE__LAYER_HH__
#define __ENGINE__LAYER_HH__

#include "../Events/Event.hh"

namespace Esper
{
    class Layer
    {
    public:
        virtual ~Layer() {}

        virtual void attach() {}
        virtual void detach() {}
        virtual void update() {}
        virtual void handle_event(Event &event) {}
    };

} // namespace Esper

#endif /* __ENGINE__LAYER_HH__ */
