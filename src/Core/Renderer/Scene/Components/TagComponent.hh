#ifndef SCENE_COMPONENTS_TAG_COMPONENT_HH
#define SCENE_COMPONENTS_TAG_COMPONENT_HH

#include "esppch.hh"

namespace esp
{
  struct TagComponent
  {
    std::string m_tag;

    TagComponent() = default;
    TagComponent(const std::string& tag) : m_tag{ tag } {}
  };
} // namespace esp

#endif // SCENE_COMPONENTS_TAG_COMPONENT_HH
