#include "Animation.hh"
#include "Core/Renderer/Utils/AssimpUtils.hh"
#include "Core/Resources/Systems/ResourceSystem.hh"

namespace esp
{

  Animation::Animation(aiAnimation* animation, NModel& model)
  {
    m_duration         = animation->mDuration;
    m_ticks_per_second = animation->mTicksPerSecond;

    // read_hierarchy_data(m_root_node, scene->mRootNode);
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

  void Animation::read_missing_bones(const aiAnimation* animation, NModel& model)
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

  // void Animation::read_hierarchy_data(HierarchyNodeData& dest, const aiNode* src)
  // {
  //   ESP_ASSERT(src, "Reading hierarchy data is uncorrect");

  //   dest.name           = src->mName.data;
  //   dest.transformation = AssimpUtils::convert_assimp_mat_to_glm_mat4(src->mTransformation);
  //   // dest.transformation = glm::mat4(1);
  //   dest.children_count = src->mNumChildren;

  //   for (int i = 0; i < src->mNumChildren; i++)
  //   {
  //     HierarchyNodeData new_data = {};
  //     read_hierarchy_data(new_data, src->mChildren[i]);

  //     dest.children.push_back(new_data);
  //   }
  // }
} // namespace esp
