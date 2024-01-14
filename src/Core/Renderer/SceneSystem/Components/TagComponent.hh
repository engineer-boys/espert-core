#ifndef SCENE_COMPONENTS_TAG_COMPONENT_HH
#define SCENE_COMPONENTS_TAG_COMPONENT_HH

#include "esppch.hh"

namespace esp
{
  /// @brief ECS component that attaches a string tag.
  struct TagComponent
  {
    /// @brief A string tag.
    std::string m_tag;

    /// @brief Default constructor.
    TagComponent() = default;
    /// @brief Constructor setting tag.
    /// @param tag A string tag.
    TagComponent(const std::string& tag) : m_tag{ tag } {}
  };
} // namespace esp

#endif // SCENE_COMPONENTS_TAG_COMPONENT_HH
