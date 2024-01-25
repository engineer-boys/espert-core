#include "Animator.hh"

namespace esp
{
  Animator::Animator()
  {
    m_current_bone_matrices = nullptr;
    m_current_time          = 0.0;

    for (int i = 0; i < MAX_BONES; i++)
    {
      m_final_bone_matrices[i] = glm::mat4(1.0f);
    }
  }

  Animator::~Animator()
  {
    // Nothing
  }

  void Animator::update_animation(float dt, Model* model)
  {
    if (!m_current_animation) return;

    m_current_time += m_current_animation->get_ticks_per_second() * dt;
    auto to_break = check_animation_period();

    m_current_time = fmod(m_current_time, m_current_animation->get_duration());

    calculate_bone_transform(model, &(model->m_root_node), glm::mat4(1.0f));
    if (to_break) { m_current_animation = nullptr; }
  }

  void Animator::play_animation(Model* model, uint32_t animation_index, AnimationPeriodType type, uint32_t cycles_count)
  {
    m_current_animation = model->m_animations[animation_index].get();
    m_current_time      = 0.0f;

    m_period.m_type        = type;
    m_period.m_cycle_to_do = type == AnimationPeriodType::ONLY_ONCE ? 1 : cycles_count;

    caclucate_bote_transform_moment_zero(model, &(model->m_root_node), glm::mat4(1.0f));
    m_current_bone_matrices = m_final_bone_matrices;
  }

  void Animator::calculate_bone_transform(Model* model, ModelNode* node, glm::mat4 parent_transform)
  {
    std::string node_name    = node->m_name;
    glm::mat4 node_transform = node->m_transformation;

    Channel* channel = m_current_animation->find_channel(node_name);

    if (channel)
    {
      channel->update(m_current_time);
      node_transform = channel->get_local_transform();
    }

    glm::mat4 global_transformation = parent_transform * node_transform;

    auto bone_info_map = m_current_animation->get_bone_id_map();
    if (bone_info_map.find(node_name) != bone_info_map.end())
    {
      int index                    = bone_info_map[node_name].m_id;
      glm::mat4 bone_space_matrix  = bone_info_map[node_name].m_bone_space_matrix;
      m_final_bone_matrices[index] = global_transformation * bone_space_matrix;
    }

    for (int child_idx = 0; child_idx < node->m_children.size(); child_idx++)
    {
      calculate_bone_transform(model, model->m_nodes[node->m_children[child_idx]], global_transformation);
    }
  }

  void Animator::caclucate_bote_transform_moment_zero(Model* model, ModelNode* node, glm::mat4 parent_transform)
  {
    std::string node_name    = node->m_name;
    glm::mat4 node_transform = node->m_transformation;

    Channel* channel = m_current_animation->find_channel(node_name);

    if (channel)
    {
      channel->set_moment_zero();
      node_transform = channel->get_local_transform();
    }

    glm::mat4 global_transformation = parent_transform * node_transform;

    auto bone_info_map = m_current_animation->get_bone_id_map();
    if (bone_info_map.find(node_name) != bone_info_map.end())
    {
      int index                    = bone_info_map[node_name].m_id;
      glm::mat4 bone_space_matrix  = bone_info_map[node_name].m_bone_space_matrix;
      m_final_bone_matrices[index] = global_transformation * bone_space_matrix;
    }

    for (int child_idx = 0; child_idx < node->m_children.size(); child_idx++)
    {
      caclucate_bote_transform_moment_zero(model, model->m_nodes[node->m_children[child_idx]], global_transformation);
    }
  }

  bool Animator::check_animation_period()
  {
    if (m_period.m_type == AnimationPeriodType::NON_STOP) return false;

    if (m_current_time > m_current_animation->get_duration()) { m_period.m_cycle_to_do -= 1; }

    if (m_period.m_cycle_to_do == 0) { return true; }

    return false;
  }
} // namespace esp
