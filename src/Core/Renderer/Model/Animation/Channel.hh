#ifndef SCENE_ANIMATION_BONE_HH
#define SCENE_ANIMATION_BONE_HH

#include "esppch.hh"

namespace esp
{
  struct PositionModifier
  {
    glm::vec3 position;
    float time_stamp;
  };

  struct RotationModifier
  {
    glm::quat orientation;
    float time_stamp;
  };

  struct ScaleModifier
  {
    glm::vec3 scale;
    float time_stamp;
  };

  class Channel
  {
   private:
    std::vector<PositionModifier> m_positions;
    std::vector<RotationModifier> m_rotations;
    std::vector<ScaleModifier> m_scales;

    int m_nr_positions;
    int m_nr_rotations;
    int m_nr_scalings;

    glm::mat4 m_local_transform;
    std::string m_name;
    const int m_id;

   private:
    float get_scale_factor(float last_time_stamp, float next_time_stamp, float animation_time) const;

    glm::mat4 interpolate_position(float animation_time);
    glm::mat4 interpolate_rotation(float animation_time);
    glm::mat4 interpolate_scaling(float animation_time);

   public:
    Channel(const std::string& name, int ID, const aiNodeAnim* channel);

    PREVENT_COPY(Channel)

    void update(float animation_time);
    void set_moment_zero();

    inline glm::mat4 get_local_transform() { return m_local_transform; }
    inline std::string get_bone_name() const { return m_name; }
    inline int get_bone_id() { return m_id; }

    int get_position_index(float animation_time) const;
    int get_rotation_index(float animation_time) const;
    int get_scale_index(float animation_time) const;
  };
} // namespace esp

#endif // SCENE_ANIMATION_BONE_HH
