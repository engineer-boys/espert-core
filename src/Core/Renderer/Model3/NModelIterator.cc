#include "NModelIterator.hh"

#include "NModel.hh"

namespace esp
{
  NModelIterator::NModelIterator(NModel* model)
  {
    if (!model)
    {
      m_model        = nullptr;
      m_current_node = nullptr;
      return;
    }

    m_model = model;

    model->m_vertex_buffer->attach();
    model->m_index_buffer->attach();

    m_current_node = &(model->m_root_node);
    for (auto node : m_current_node->m_children)
    {
      m_stack.push(node);
    }
  }

  NModelIterator::~NModelIterator()
  { /* nothing */
  }

  bool NModelIterator::operator!=(const NModelIterator& other) { return m_current_node != other.m_current_node; }

  NNodeInfo NModelIterator::operator*() { return NNodeInfo{ .m_current_node = m_current_node }; }

  void NModelIterator::operator++()
  {
    do
    {
      m_current_node = nullptr;
      if (m_stack.size() > 0)
      {
        m_current_node = m_model->m_nodes[m_stack.top()];
        m_stack.pop();

        for (auto node : m_current_node->m_children)
        {
          m_stack.push(node);
        }
      }
    } while (m_current_node && !m_current_node->m_mesh_node);
  }
} // namespace esp
