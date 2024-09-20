#ifndef ESPERT_SANDBOX_NODECOMPONENT_HH
#define ESPERT_SANDBOX_NODECOMPONENT_HH

#include "esppch.hh"

#include "Core/Renderer/SceneSystem/Action.hh"
#include "Core/Renderer/SceneSystem/ComponentManagers/NodeManager.hh"

namespace esp
{
  class Entity;

  struct NodeComponent
  {
    entt::entity m_parent;
    std::vector<entt::entity> m_children;
    entt::entity m_handle;

    NodeComponent(entt::entity parent, entt::entity entity);
    NodeComponent(entt::entity entity);
    ~NodeComponent() = default;

    void add_child(entt::entity child_id);
    void remove_child(entt::entity child_id);
    void set_parent(entt::entity parent_id);

    // ---------- Template functions ----------
    template<typename... Args> void act(action::Action<void(NodeComponent&, Args...)> f, Args&&... args)
    {
      f(NodeManager::get_node(this->m_handle), std::forward<Args>(args)...);
      for (auto& node : m_children)
      {
        NodeManager::get_node(node).act(f, std::forward<Args>(args)...);
      }
    }

   private:
    std::vector<entt::entity>::iterator find_child(entt::entity child_id);
  };
} // namespace esp

#endif // ESPERT_SANDBOX_NODECOMPONENT_HH
