#ifndef ESPERT_CORE_LAYERS_LAYERSTACK_HH_
#define ESPERT_CORE_LAYERS_LAYERSTACK_HH_

#include "Layer.hh"
#include "esppch.hh"

namespace esp
{
  class LayerStack
  {
   private:
    std::vector<Layer*> _m_layers;
    std::vector<Layer*>::iterator _m_head_of_normal_layers;

   public:
    LayerStack();
    ~LayerStack();

    void push_layer(Layer* layer);
    void push_overlayer(Layer* layer);

    void pop_layer(Layer* layer);
    void pop_overlayer(Layer* layer);

    std::vector<Layer*>::iterator begin() { return _m_layers.begin(); }
    std::vector<Layer*>::iterator end() { return _m_layers.end(); }
  };
} // namespace esp

#endif // ESPERT_CORE_LAYERS_LAYERSTACK_HH_
