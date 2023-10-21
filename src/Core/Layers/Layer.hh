#ifndef ESPERT_CORE_LAYERS_LAYER_HH
#define ESPERT_CORE_LAYERS_LAYER_HH

#include "../Events/Event.hh"

namespace esp
{
  class Layer
  {
   public:
    virtual ~Layer() {}

    virtual void attach() {}
    virtual void detach() {}
    virtual void update() {}
    virtual void handle_event(Event& event) {}
  };
} // namespace esp

#endif // ESPERT_CORE_LAYERS_LAYER_HH
