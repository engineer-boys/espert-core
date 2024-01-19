#ifndef RENDERER_NMODEL_NMODEL_ITERATOR_HH
#define RENDERER_NMODEL_NMODEL_ITERATOR_HH

#include "esppch.hh"

#include "Core/Renderer/Model3/Mesh/ModelParts.hh"

#include <stack>

namespace esp
{
  class NModel;

  struct NNodeInfo
  {
    NNode* m_current_node;
  };

  struct NModelIterator
  {
    NModel* m_model;
    std::stack<uint32_t> m_stack;
    NNode* m_current_node;

    NModelIterator(NModel* model);
    ~NModelIterator();

    bool operator!=(const NModelIterator& other);

    NNodeInfo operator*();
    void operator++();
  };
} // namespace esp

#endif // RENDERER_NMODEL_NMODEL_ITERATOR_HH
