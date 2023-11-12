#ifndef LAYERS_INPUT_LAYER_HH
#define LAYERS_INPUT_LAYER_HH

#include "Core/Events/Events.hh"
#include "Layer.hh"

namespace esp
{
  class InputLayer : public Layer
  {
   public:
    InputLayer() = default;

    void handle_event(Event& event, float dt) override;

   private:
    bool key_pressed_event_handler(KeyPressedEvent& event, float dt);
    bool mouse_move_event_handler(MouseMovedEvent& event, float dt);
    bool mouse_scroll_event_handler(MouseScrolledEvent& event);
  };
} // namespace esp

#endif // LAYERS_INPUT_LAYER_HH
