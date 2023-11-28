#ifndef SCENE_COMPONENTS_MODEL_COMPONENT_HH
#define SCENE_COMPONENTS_MODEL_COMPONENT_HH

#include "esppch.hh"

#include "Core/Scene/Model.hh"

namespace esp
{
  //  class Model;

  struct ModelComponent
  {
    std::shared_ptr<Model> m_model_handle;

    ModelComponent() = default;
    ModelComponent(std::shared_ptr<Model> model) : m_model_handle{ std::move(model) } {}

    // TODO: this operator will be used to group different models in Renderer
    bool operator==(const ModelComponent& other) const { return m_model_handle.get() == other.m_model_handle.get(); }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_MODEL_COMPONENT_HH
