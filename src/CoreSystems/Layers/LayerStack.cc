#include "LayerStack.hh"

namespace esp
{
    LayerStack::LayerStack()
    {
        _m_head_of_normall_layers = _m_layers.begin();
    }

    LayerStack::~LayerStack()
    {
        for (Layer *layer : _m_layers)
        {
            delete layer;
        }
    }

    void LayerStack::push_layer(Layer *layer)
    {
        _m_head_of_normall_layers = _m_layers.emplace(_m_head_of_normall_layers, layer);
    }

    void LayerStack::push_overlayer(Layer *layer)
    {
        _m_layers.emplace_back(layer);
    }

    void LayerStack::pop_layer(Layer *layer)
    {
        auto iter = std::find(_m_layers.begin(), _m_layers.end(), layer);
        if (iter != _m_layers.end())
        {
            _m_layers.erase(iter);
            _m_head_of_normall_layers--;
        }
    }

    void LayerStack::pop_overlayer(Layer *layer)
    {
        auto iter = std::find(_m_layers.begin(), _m_layers.end(), layer);
        if (iter != _m_layers.end())
        {
            _m_layers.erase(iter);
        }
    }

} // namespace Espert
