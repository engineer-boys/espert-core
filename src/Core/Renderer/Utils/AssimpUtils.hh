#ifndef SCENE_ASSIMP_UTILS_HH
#define SCENE_ASSIMP_UTILS_HH

#include "esppch.hh"

namespace esp
{
  class AssimpUtils
  {
   public:
    static glm::vec4 convert_assimp_color_to_glm_vec4(const aiColor4D& vec)
    {
      return glm::vec4(vec.r, vec.g, vec.b, vec.a);
    }

    static glm::vec3 convert_assimp_vec_to_glm_vec3(const aiVector3D& vec) { return glm::vec3(vec.x, vec.y, vec.z); }

    static glm::vec2 convert_assimp_vec_to_glm_vec2(const aiVector2D& vec) { return glm::vec2(vec.x, vec.y); }

    static glm::quat convert_assimp_quat_to_glm_quat(const aiQuaternion& quat)
    {
      return glm::quat(quat.w, quat.x, quat.y, quat.z);
    }

    static glm::mat4 convert_assimp_mat_to_glm_mat4(const aiMatrix4x4& mat)
    {
      glm::mat4 glm_mat;

      glm_mat[0][0] = mat.a1;
      glm_mat[1][0] = mat.a2;
      glm_mat[2][0] = mat.a3;
      glm_mat[3][0] = mat.a4;

      glm_mat[0][1] = mat.b1;
      glm_mat[1][1] = mat.b2;
      glm_mat[2][1] = mat.b3;
      glm_mat[3][1] = mat.b4;

      glm_mat[0][2] = mat.c1;
      glm_mat[1][2] = mat.c2;
      glm_mat[2][2] = mat.c3;
      glm_mat[3][2] = mat.c4;

      glm_mat[0][3] = mat.d1;
      glm_mat[1][3] = mat.d2;
      glm_mat[2][3] = mat.d3;
      glm_mat[3][3] = mat.d4;

      return glm_mat;
    }
  };
} // namespace esp

#endif // SCENE_ASSIMP_UTILS_HH
