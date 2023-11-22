#include <utility>

#ifndef SCENE_SCENE_NODE_HH
#define SCENE_SCENE_NODE_HH

namespace esp
{
  class Entity;

  class SceneNode
  {
   private:
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
    inline Entity& get_entity() { return *m_entity; }

    void add_child(const std::shared_ptr<SceneNode>& child);

    template<typename Func, typename... Args> void act(const Func& f, Args&&... args)
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
