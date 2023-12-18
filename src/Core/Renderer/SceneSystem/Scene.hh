#ifndef SCENE_SCENE_HH
#define SCENE_SCENE_HH

#include "Core/Renderer/Camera.hh"
#include "Node.hh"

#include "esppch.hh"

namespace esp
{
  class Entity;

  class Scene
  {
   private:
    entt::registry m_registry;
    std::unique_ptr<Node> m_root_node;

    static Camera* s_current_camera;
    std::vector<std::shared_ptr<Camera>> m_cameras;

   public:
    static std::shared_ptr<Scene> create();

    PREVENT_COPY(Scene);

    ~Scene() = default;

    std::shared_ptr<Entity> create_entity(const std::string& name = std::string());
    void destroy_entity(Entity& entity);

    inline void add_camera(std::shared_ptr<Camera> camera) { m_cameras.push_back(std::move(camera)); }

    inline std::shared_ptr<Camera> get_camera(uint32_t index)
    {
      ESP_ASSERT(index < m_cameras.size(), "Index was out of range")
      return m_cameras[index];
    }
    inline Node& get_root() { return *m_root_node; }

    template<typename... Args> auto& view() const { return m_registry.view<Args...>(); }

    inline static void set_current_camera(Camera* camera) { s_current_camera = camera; }
    inline static Camera* get_current_camera() { return s_current_camera; }

   private:
    Scene() : m_root_node{ Node::create_root() } {}

    friend class Entity;
  };
} // namespace esp

#endif // SCENE_SCENE_HH
