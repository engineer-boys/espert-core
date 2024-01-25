#ifndef SCENE_ANIMATION_BONE_INFO_HH
#define SCENE_ANIMATION_BONE_INFO_HH

#include "esppch.hh"

namespace esp
{

  struct BoneInfo
  {
    uint32_t m_id;
    glm::mat4 m_bone_space_matrix;
  };

} // namespace esp

#endif /* SCENE_ANIMATION_BONE_INFO_HH */
