#ifndef ESPERT_SANDBOX_NODECOMPONENT_HH
#define ESPERT_SANDBOX_NODECOMPONENT_HH

#include "esppch.hh"

#include "Core/Renderer/SceneSystem/Action.hh"
#include "Core/Renderer/SceneSystem/TransformManager.hh"

namespace esp
{
  class Entity;

  struct NodeComponent
  {
   private:
    static Scene* s_scene;

   public:
    entt::entity m_parent;
    std::vector<entt::entity> m_children;
    entt::entity m_handle;

    NodeComponent(entt::entity parent, entt::entity entity) : m_handle{ entity }
    {
      get_node(parent).add_child(this->m_handle);
    }
    NodeComponent(entt::entity entity) : m_parent{ entt::null }, m_handle{ entity } {}
    ~NodeComponent() = default;

    void add_child(entt::entity child_id);
    void remove_child(entt::entity child_id);
    void set_parent(entt::entity parent_id);

    // ---------- Template functions ----------
    template<typename... Args> void act(action::Action<void(NodeComponent&, Args...)> f, Args&&... args)
    {
      f(get_node(this->m_handle), std::forward<Args>(args)...);
      for (auto& node : m_children)
      {
        get_node(node).act(f, std::forward<Args>(args)...);
      }
    }

    static NodeComponent& get_node(entt::entity id);
    static NodeComponent& get_node(uint32_t id);

   private:
    std::vector<entt::entity>::iterator find_child(entt::entity child_id);

    friend class Scene;
  };

  inline Scene* NodeComponent::s_scene = nullptr;

  inline void NodeComponent::add_child(entt::entity child_id)
  {
    auto found = find_child(child_id);
    if (found != m_children.end()) { return; }

    get_node(child_id).set_parent(this->m_handle);
    m_children.emplace_back(child_id);
  }

  inline void NodeComponent::remove_child(entt::entity child_id)
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

    get_node(child_id).m_parent = entt::null;
    m_children.erase(found);
  }

  inline void NodeComponent::set_parent(entt::entity parent_id)
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

  inline std::vector<entt::entity>::iterator NodeComponent::find_child(entt::entity child_id)
  {
    return std::find_if(m_children.begin(), m_children.end(), [&](const auto& item) { return item == child_id; });
  }

  inline NodeComponent& NodeComponent::get_node(entt::entity id) { return s_scene->get_component<NodeComponent>(id); }
  inline NodeComponent& NodeComponent::get_node(uint32_t id) { return s_scene->get_component<NodeComponent>(id); }

} // namespace esp

#endif // ESPERT_SANDBOX_NODECOMPONENT_HH
