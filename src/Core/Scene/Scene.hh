#ifndef SCENE_SCENE_HH
#define SCENE_SCENE_HH

#include "SceneNode.hh"

#include "esppch.hh"

namespace esp
{
  class Entity;

  class Scene : public std::enable_shared_from_this<Scene>
  {
   private:
    entt::registry m_registry;
    std::unique_ptr<SceneNode> m_root_node;

   public:
    static std::shared_ptr<Scene> create();

    PREVENT_COPY(Scene);

    ~Scene() = default;

    std::shared_ptr<Entity> create_entity(const std::string& name = std::string());
    void destroy_entity(Entity& entity);

    inline SceneNode& get_root() { return *m_root_node; }

    template<typename... Args> auto& view() const { return m_registry.view<Args...>(); }

   private:
    Scene() : m_root_node{ SceneNode::create_root() } {}

    friend class Entity;
  };
} // namespace esp

#endif // SCENE_SCENE_HH
