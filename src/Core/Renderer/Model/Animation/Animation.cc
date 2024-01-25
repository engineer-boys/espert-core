#include "Animation.hh"
#include "Core/Renderer/Utils/AssimpUtils.hh"
#include "Core/Resources/Systems/ResourceSystem.hh"

namespace esp
{

  Animation::Animation(aiAnimation* animation, Model& model)
  {
    m_duration         = animation->mDuration;
    m_ticks_per_second = animation->mTicksPerSecond;

    read_missing_bones(animation, model);
  }

  Animation::~Animation()
  {
    for (auto& channel : m_channels)
    {
      delete channel;
    }

    m_channels.clear();
  }

  Channel* Animation::find_channel(const std::string& name)
  {
    auto iter = std::find_if(m_channels.begin(),
                             m_channels.end(),
                             [&](const Channel* channel) { return channel->get_bone_name() == name; });

    if (iter == m_channels.end()) { return nullptr; }
    else { return *iter; }
  }

  void Animation::read_missing_bones(const aiAnimation* animation, Model& model)
  {
    int channel_count = animation->mNumChannels;

    auto& bone_info_map  = model.get_bone_info_map();
    uint32_t& bone_count = model.get_bone_count();

    for (int channel_idx = 0; channel_idx < channel_count; channel_idx++)
    {
      auto channel         = animation->mChannels[channel_idx];
      std::string boneName = channel->mNodeName.data;

      if (bone_info_map.find(boneName) == bone_info_map.end())
      {
        bone_info_map[boneName].m_id = bone_count;
        bone_count++;
      }
      m_channels.push_back(new Channel(channel->mNodeName.data, bone_info_map[channel->mNodeName.data].m_id, channel));
    }

    m_bone_info_map = bone_info_map;
  }
} // namespace esp
