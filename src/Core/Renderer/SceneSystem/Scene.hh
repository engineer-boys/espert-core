#ifndef SCENE_SCENE_HH
#define SCENE_SCENE_HH

#include "Core/Renderer/Cameras/Camera.hh"
#include "Node.hh"

#include "esppch.hh"

namespace esp
{
  class Entity;

  /// @brief Graph of Nodes that contains all of objects used in rendering.
  class Scene
  {
   private:
    entt::registry m_registry;
    std::shared_ptr<Node> m_root_node;

    static Camera* s_current_camera;
    std::vector<std::shared_ptr<Camera>> m_cameras;

   public:
    /// @brief Creates instance of a Scene.
    /// @return Shared pointer to instance of a Scene.
    static std::shared_ptr<Scene> create();

    PREVENT_COPY(Scene);

    /// @brief Default destructor.
    ~Scene() = default;

    /// @brief Creates a new Entity in current Scene.
    /// @param name Name of the Entity.
    /// @return Shared pointer to created Entity.
    std::shared_ptr<Entity> create_entity(const std::string& name = std::string());
    /// @brief Removes Entity from Scene and destroys it.
    /// @param entity Reference to an Entity to be destroyed.
    void destroy_entity(Entity& entity);

    /// @brief Adds a Camera to the Scene.
    /// @param camera Shared pointer to the Camera to be added to current Scene.
    inline void add_camera(std::shared_ptr<Camera> camera) { m_cameras.push_back(std::move(camera)); }

    /// @brief Returns Scene's Camera by it's index.
    /// @param index Index of the Camera to be returned.
    /// @return Shared pointer to Camera at index.
    inline std::shared_ptr<Camera> get_camera(uint32_t index)
    {
      ESP_ASSERT(index < m_cameras.size(), "Index was out of range")
      return m_cameras[index];
    }
    inline Node& get_root() { return *m_root_node; }

    /// @brief Returns a view for the given component.
    /// @tparam ...Args Type of component used to construct the view.
    /// @return View for the given component.
    template<typename... Args> auto get_view() const { return m_registry.view<Args...>(); }

    /// @brief Sets the current Camera.
    /// @param camera Pointer to he Camera to be set as current one.
    inline static void set_current_camera(Camera* camera) { s_current_camera = camera; }
    /// @brief Returns pointer to the current Camera.
    /// @return Pointer to the current Camera.
    inline static Camera* get_current_camera() { return s_current_camera; }

    /// @brief Draws each Node on scene graph that has ModelComponent
    void draw(); // TODO: move draw logic to Renderer class

   private:
    Scene() : m_root_node{ Node::create_root(this) } {}

    friend class Entity;
  };
} // namespace esp

#endif // SCENE_SCENE_HH
