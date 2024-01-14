#ifndef ESPERT_CORE_LAYERS_LAYERSTACK_HH
#define ESPERT_CORE_LAYERS_LAYERSTACK_HH

#include "Layer.hh"
#include "esppch.hh"

namespace esp
{
  /// @brief Stack containing all of engine layers.
  class LayerStack
  {
   private:
    std::vector<Layer*> m_layers;
    std::vector<Layer*>::iterator m_head_of_normal_layers;

   public:
    /// @brief Constructor sets head of layers.
    LayerStack();
    /// @brief Destructor deletes all layers.
    ~LayerStack();

    /// @brief Pushes layer to stack.
    /// @param layer Pointer to the layer to be pushed.
    void push_layer(Layer* layer);
    /// @brief Push special layer above normal layers.
    /// @param layer Pointer to the layer to be pushed.
    void push_overlayer(Layer* layer);

    /// @brief Remove layer from stack.
    /// @param layer Pointer to the layer to be pushed.
    void pop_layer(Layer* layer);
    /// @brief Remove one of the special layers.
    /// @param layer Pointer to the layer to be pushed.
    void pop_overlayer(Layer* layer);

    /// @brief LayerStack iterator pointing to the first layer.
    /// @return LayerStack iterator pointing to the first layer.
    std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
    /// @brief LayerStack iterator pointing behind last layer.
    /// @return LayerStack iterator pointing behind last layer.
    std::vector<Layer*>::iterator end() { return m_layers.end(); }
  };
} // namespace esp

#endif // ESPERT_CORE_LAYERS_LAYERSTACK_HH
