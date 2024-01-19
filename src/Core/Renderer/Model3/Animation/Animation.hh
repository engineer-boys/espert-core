#ifndef SCENE_ANIMATION_ANIMATION_HH
#define SCENE_ANIMATION_ANIMATION_HH

#include "esppch.hh"

#include "BoneInfo.hh"
#include "Channel.hh"

#include "Core/Renderer/Model3/NModel.hh"

namespace esp
{
  class Animation
  {
   private:
    float m_duration;
    int m_ticks_per_second;

    std::vector<Channel*> m_channels;
    std::map<std::string, BoneInfo> m_bone_info_map;

   private:
    void read_missing_bones(const aiAnimation* animation, NModel& model);

   public:
    Animation(aiAnimation* animation, NModel& model);

    PREVENT_COPY(Animation)

    ~Animation();

    Channel* find_channel(const std::string& name);

    inline float get_ticks_per_second() const { return m_ticks_per_second; }
    inline float get_duration() const { return m_duration; }
    inline const std::map<std::string, BoneInfo>& get_bone_id_map() { return m_bone_info_map; }
  };
} // namespace esp

#endif // SCENE_ANIMATION_ANIMATION_HH
