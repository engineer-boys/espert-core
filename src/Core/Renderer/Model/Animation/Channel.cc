#include "Channel.hh"

#include "Core/Renderer/Utils/AssimpUtils.hh"

namespace esp
{
  Channel::Channel(const std::string& name, int ID, const aiNodeAnim* channel) :
      m_name{ name }, m_id{ ID }, m_local_transform{ 1.0f }
  {
    m_nr_positions = channel->mNumPositionKeys;

    for (int positionIndex = 0; positionIndex < m_nr_positions; ++positionIndex)
    {
      aiVector3D ai_position = channel->mPositionKeys[positionIndex].mValue;
      float time_stamp       = channel->mPositionKeys[positionIndex].mTime;

      PositionModifier data = {};
      data.position         = AssimpUtils::convert_assimp_vec_to_glm_vec3(ai_position);
      data.time_stamp       = time_stamp;

      m_positions.push_back(data);
    }

    m_nr_rotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_nr_rotations; ++rotationIndex)
    {
      aiQuaternion ai_orientation = channel->mRotationKeys[rotationIndex].mValue;
      float time_stamp            = channel->mRotationKeys[rotationIndex].mTime;

      RotationModifier data = {};
      data.orientation      = AssimpUtils::convert_assimp_quat_to_glm_quat(ai_orientation);
      data.time_stamp       = time_stamp;

      m_rotations.push_back(data);
    }

    m_nr_scalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_nr_scalings; ++keyIndex)
    {
      aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
      float time_stamp = channel->mScalingKeys[keyIndex].mTime;

      ScaleModifier data = {};
      data.scale         = AssimpUtils::convert_assimp_vec_to_glm_vec3(scale);
      data.time_stamp    = time_stamp;

      m_scales.push_back(data);
    }
  }

  void Channel::update(float animation_time)
  {
    glm::mat4 translation = interpolate_position(animation_time);
    glm::mat4 rotation    = interpolate_rotation(animation_time);
    glm::mat4 scale       = interpolate_scaling(animation_time);

    m_local_transform = translation * rotation * scale;
  }

  void Channel::set_moment_zero()
  {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_positions[0].position);

    auto rotation_quad = glm::normalize(m_rotations[0].orientation);
    glm::mat4 rotation = glm::mat4_cast(rotation_quad);

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scales[0].scale);

    m_local_transform = translation * rotation * scale;
  }

  int Channel::get_position_index(float animation_time) const
  {
    for (int index = 0; index < m_nr_positions - 1; ++index)
    {
      if (animation_time < m_positions[index + 1].time_stamp) return index;
    }
    ESP_ASSERT(0, "Possition cannot find index");
  }

  int Channel::get_rotation_index(float animation_time) const
  {
    for (int index = 0; index < m_nr_rotations - 1; ++index)
    {
      if (animation_time < m_rotations[index + 1].time_stamp) return index;
    }
    ESP_ASSERT(0, "Rotation cannot find index");
  }

  int Channel::get_scale_index(float animation_time) const
  {
    for (int index = 0; index < m_nr_scalings - 1; ++index)
    {
      if (animation_time < m_scales[index + 1].time_stamp) return index;
    }
    ESP_ASSERT(0, "Scale cannot find index");
  }

  float Channel::get_scale_factor(float last_time_stamp, float next_time_stamp, float animation_time) const
  {
    float mid_way_length    = animation_time - last_time_stamp;
    float frames_difference = next_time_stamp - last_time_stamp;

    return mid_way_length / frames_difference;
  }

  glm::mat4 Channel::interpolate_position(float animation_time)
  {
    if (1 == m_nr_positions) { return glm::translate(glm::mat4(1.0f), m_positions[0].position); }

    int p0_index = get_position_index(animation_time);
    int p1_index = p0_index + 1;

    float scale_factor =
        get_scale_factor(m_positions[p0_index].time_stamp, m_positions[p1_index].time_stamp, animation_time);
    glm::vec3 final_position = glm::mix(m_positions[p0_index].position, m_positions[p1_index].position, scale_factor);

    return glm::translate(glm::mat4(1.0f), final_position);
  }

  glm::mat4 Channel::interpolate_rotation(float animation_time)
  {
    if (1 == m_nr_rotations)
    {
      auto rotation = glm::normalize(m_rotations[0].orientation);
      return glm::mat4_cast(rotation);
    }

    int p0_index = get_rotation_index(animation_time);
    int p1_index = p0_index + 1;

    float scale_factor =
        get_scale_factor(m_rotations[p0_index].time_stamp, m_rotations[p1_index].time_stamp, animation_time);
    glm::quat final_rotation =
        glm::slerp(m_rotations[p0_index].orientation, m_rotations[p1_index].orientation, scale_factor);
    final_rotation = glm::normalize(final_rotation);

    return glm::mat4_cast(final_rotation);
  }

  glm::mat4 Channel::interpolate_scaling(float animation_time)
  {
    if (1 == m_nr_scalings) { return glm::scale(glm::mat4(1.0f), m_scales[0].scale); }

    int p0_index = get_scale_index(animation_time);
    int p1_index = p0_index + 1;

    float scale_factor = get_scale_factor(m_scales[p0_index].time_stamp, m_scales[p1_index].time_stamp, animation_time);
    glm::vec3 final_scale = glm::mix(m_scales[p0_index].scale, m_scales[p1_index].scale, scale_factor);

    return glm::scale(glm::mat4(1.0f), final_scale);
  }
} // namespace esp
