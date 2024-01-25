#ifndef RENDERER_MODEL_MODEL_ITERATOR_HH
#define RENDERER_MODEL_MODEL_ITERATOR_HH

#include "esppch.hh"

#include "Core/Renderer/Model/Mesh/ModelParts.hh"

#include <stack>

namespace esp
{
  class Model;

  struct NNodeInfo
  {
    ModelNode* m_current_node;
  };

  struct ModelIterator
  {
    Model* m_model;
    std::stack<uint32_t> m_stack;
    ModelNode* m_current_node;

    ModelIterator(Model* model);
    ~ModelIterator();

    bool operator!=(const ModelIterator& other);

    NNodeInfo operator*();
    void operator++();
  };
} // namespace esp

#endif // RENDERER_MODEL_MODEL_ITERATOR_HH
