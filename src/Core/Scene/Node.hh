#ifndef SCENE_SCENE_NODE_HH
#define SCENE_SCENE_NODE_HH

#include "Action.hh"

namespace esp
{
  class Entity;

  class Node
  {
   private:
    std::shared_ptr<Node> m_parent;
    std::vector<std::shared_ptr<Node>> m_children;
    std::shared_ptr<Entity> m_entity;

   public:
    static std::shared_ptr<Node> create();
    static std::shared_ptr<Node> create(std::shared_ptr<Entity> entity);

   private:
    static std::unique_ptr<Node> create_root();

   public:
    PREVENT_COPY(Node)

    void attach_entity(std::shared_ptr<Entity> entity);

    void set_parent(std::shared_ptr<Node> parent);
    void add_child(std::shared_ptr<Node> child);

    Node* get_parent();
    Entity* get_entity();

    template<typename... Args> void act(Action<void(Node*, Args...)> f, Args&&... args)
    {
      f(this, std::forward<Args>(args)...);
      for (auto& node : m_children)
      {
        node->act(f, std::forward<Args>(args)...);
      }
    }

   private:
    Node() : m_entity{ nullptr } {}
    Node(const std::shared_ptr<Entity>& entity) : m_entity{ entity } {}

    friend class Scene;
  };
} // namespace esp

#endif // SCENE_SCENE_NODE_HH
