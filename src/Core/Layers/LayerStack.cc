#include "LayerStack.hh"

namespace esp
{
  LayerStack::LayerStack() { m_head_of_normal_layers = m_layers.begin(); }

  LayerStack::~LayerStack()
  {
    for (Layer* layer : m_layers)
    {
      delete layer;
    }
  }

  void LayerStack::push_layer(Layer* layer)
  {
    m_head_of_normal_layers = m_layers.emplace(m_head_of_normal_layers, layer);
  }

  void LayerStack::push_overlayer(Layer* layer)
  {
    m_layers.emplace_back(layer);
    if (m_layers.size() == 1) { m_head_of_normal_layers = m_layers.begin(); } // TODO: fix layer stack Dawid!
  }

  void LayerStack::pop_layer(Layer* layer)
  {
    auto iter = std::find(m_layers.begin(), m_layers.end(), layer);
    if (iter != m_layers.end())
    {
      m_layers.erase(iter);
      m_head_of_normal_layers--;
    }
  }

  void LayerStack::pop_overlayer(Layer* layer)
  {
    auto iter = std::find(m_layers.begin(), m_layers.end(), layer);
    if (iter != m_layers.end()) { m_layers.erase(iter); }
  }
} // namespace esp
