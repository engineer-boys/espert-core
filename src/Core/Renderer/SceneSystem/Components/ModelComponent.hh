#ifndef SCENE_COMPONENTS_MODEL_COMPONENT_HH
#define SCENE_COMPONENTS_MODEL_COMPONENT_HH

#include "esppch.hh"

#include "Core/Renderer/Model/Model.hh"

namespace esp
{
  //  class Model;

  /// @brief ECS component that attaches Material.
  struct ModelComponent
  {
    /// @brief Shared pointer to model instance.
    std::shared_ptr<Model> m_model_handle;

    /// @brief Default constructor.
    ModelComponent() = default;
    /// @brief Constructor that sets shared pointer to model.
    /// @param model Shared pointer to model.
    ModelComponent(std::shared_ptr<Model> model) : m_model_handle{ std::move(model) } {}

    /// @brief Equals operator checks if two components reference the same model.
    /// @param other Other component to compare to.
    /// @return True if two components reference the same model. False otherwise.
    bool operator==(const ModelComponent& other) const { return m_model_handle.get() == other.m_model_handle.get(); }
  };
} // namespace esp

#endif // SCENE_COMPONENTS_MODEL_COMPONENT_HH
