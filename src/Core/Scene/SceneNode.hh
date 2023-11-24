#ifndef SCENE_SCENE_NODE_HH
#define SCENE_SCENE_NODE_HH

#include "Action.hh"

namespace esp
{
  class Entity;

  class SceneNode
  {
   private:
    std::shared_ptr<SceneNode> m_parent;
    std::vector<std::shared_ptr<SceneNode>> m_children;
    std::shared_ptr<Entity> m_entity;

   public:
    static std::shared_ptr<SceneNode> create();
    static std::shared_ptr<SceneNode> create(const std::shared_ptr<Entity>& entity);

   private:
    static std::unique_ptr<SceneNode> create_root();

   public:
    PREVENT_COPY(SceneNode)

    void attach_entity(const std::shared_ptr<Entity>& entity);

    void set_parent(const std::shared_ptr<SceneNode>& parent);
    void add_child(const std::shared_ptr<SceneNode>& child);

    SceneNode* get_parent();
    Entity* get_entity();

    template<typename... Args> void act(Action<void(Entity&, Args...)> f, Args&&... args)
    {
      f(*m_entity, std::forward<Args>(args)...);
      for (auto& node : m_children)
      {
        node->act(f, std::forward<Args>(args)...);
      }
    }

   private:
    SceneNode() : m_entity{ nullptr } {}
    SceneNode(const std::shared_ptr<Entity>& entity) : m_entity{ entity } {}

    friend class Scene;
  };
} // namespace esp

#endif // SCENE_SCENE_NODE_HH
