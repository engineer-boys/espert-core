#ifndef ESPERT_CORE_LAYERS_LAYER_HH
#define ESPERT_CORE_LAYERS_LAYER_HH

#include "../Events/Event.hh"

namespace esp
{
  /// @brief Abstract interface for engine's layer. It can ancaplulate part of engine's functionality.
  class Layer
  {
   public:
    /// @brief Virtual destructor.
    virtual ~Layer() {}

    /// @brief Enables layer.
    virtual void attach() {}
    /// @brief Disables layer.
    virtual void detach() {}
    /// @brief Runs layer logic.
    /// @param dt Time that has passed since last update.
    virtual void update(float dt) {}
    /// @brief Handle event by this layer.
    /// @param event Event to be handled.
    /// @param dt Time that has passed since last update.
    virtual void handle_event(Event& event, float dt) {}
  };
} // namespace esp

#endif // ESPERT_CORE_LAYERS_LAYER_HH
