#ifndef ESPERT_CORESYSTEMS_LAYERS_LAYER_HH_
#define ESPERT_CORESYSTEMS_LAYERS_LAYER_HH_

#include "../Events/Event.hh"

namespace Espert
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

} // namespace Espert

#endif // ESPERT_CORESYSTEMS_LAYERS_LAYER_HH_
