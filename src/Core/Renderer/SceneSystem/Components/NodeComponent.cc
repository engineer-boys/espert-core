#include "NodeComponent.hh"
#include "Core/Renderer/SceneSystem/ComponentManagers/TransformManager.hh"

namespace esp
{
  NodeComponent::NodeComponent(entt::entity parent, entt::entity entity) : m_handle{ entity }
  {
    NodeManager::get_node(parent).add_child(this->m_handle);
  }

  NodeComponent::NodeComponent(entt::entity entity) : m_parent{ entt::null }, m_handle{ entity } {}

  void NodeComponent::add_child(entt::entity child_id)
  {
    auto found = find_child(child_id);
    if (found != m_children.end()) { return; }

    NodeManager::get_node(child_id).set_parent(this->m_handle);
    m_children.emplace_back(child_id);
  }

  void NodeComponent::remove_child(entt::entity child_id)
  {
    auto found = find_child(child_id);
    if (found == m_children.end()) { return; }

    glm::mat4 model_mat = TransformManager::get_model_mat(child_id);
    TransformManager::set_translation(child_id, glm::vec3(model_mat[3]));
    float scale_x          = glm::length(glm::vec3(model_mat[0]));
    float scale_y          = glm::length(glm::vec3(model_mat[1]));
    float scale_z          = glm::length(glm::vec3(model_mat[2]));
    glm::mat4 rotation_mat = glm::mat3(model_mat[0] / scale_x, model_mat[1] / scale_y, model_mat[2] / scale_z);
    TransformManager::set_rotation(child_id, glm::quat_cast(rotation_mat));
    TransformManager::set_scale(child_id, { scale_x, scale_y, scale_z });

    NodeManager::get_node(child_id).m_parent = entt::null;
    m_children.erase(found);
  }

  void NodeComponent::set_parent(entt::entity parent_id)
  {
    glm::mat4 inv_model = glm::inverse(TransformManager::get_model_mat(parent_id));
    TransformManager::translate(m_handle, glm::vec3(inv_model[3]));
    float scale_x          = glm::length(glm::vec3(inv_model[0]));
    float scale_y          = glm::length(glm::vec3(inv_model[1]));
    float scale_z          = glm::length(glm::vec3(inv_model[2]));
    glm::mat4 rotation_mat = glm::mat3(inv_model[0] / scale_x, inv_model[1] / scale_y, inv_model[2] / scale_z);
    TransformManager::rotate(m_handle, glm::quat_cast(rotation_mat));
    TransformManager::scale(m_handle, { scale_x, scale_y, scale_z });

    m_parent = parent_id;
  }

  std::vector<entt::entity>::iterator NodeComponent::find_child(entt::entity child_id)
  {
    return std::find_if(m_children.begin(), m_children.end(), [&](const auto& item) { return item == child_id; });
  }
} // namespace esp