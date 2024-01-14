#ifndef SCENE_SCENE_NODE_HH
#define SCENE_SCENE_NODE_HH

#include "Action.hh"

namespace esp
{
  class Entity;

  /// @brief Node in scene graph.
  class Node
  {
   private:
    std::shared_ptr<Node> m_parent;
    std::vector<std::shared_ptr<Node>> m_children;
    std::shared_ptr<Entity> m_entity;

   public:
    /// @brief Creates an empty Node.
    /// @return Shared pointer to empty Node.
    static std::shared_ptr<Node> create();
    /// @brief Creates new Node with an Entity.
    /// @param entity Entity to add to a Node.
    /// @return Shared pointer to created Node.
    static std::shared_ptr<Node> create(std::shared_ptr<Entity> entity);

   private:
    static std::unique_ptr<Node> create_root();

   public:
    PREVENT_COPY(Node)

    /// @brief Attaches Entity to a Node.
    /// @param entity Entity to add to a Node.
    void attach_entity(std::shared_ptr<Entity> entity);

    /// @brief Sets Node's parent in scene graph.
    /// @param parent Parent of current Node.
    void set_parent(std::shared_ptr<Node> parent);
    /// @brief Adds child to Node in scene greph.
    /// @param child Child Node to be added to current Node.
    void add_child(std::shared_ptr<Node> child);

    /// @brief Returns pointer to parent Node.
    /// @return Pointer to parent Node.
    Node* get_parent();
    /// @brief Returns pointer to Entity.
    /// @return Pointer to Entity.
    Entity* get_entity();

    /// @brief Runs Action with arguments for current Node and their children.
    /// @tparam ...Args Types of Action arguments.
    /// @param f Action to be run.
    /// @param ...args Action arguments.
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
