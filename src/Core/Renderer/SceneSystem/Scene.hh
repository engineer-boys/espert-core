#ifndef SCENE_SCENE_HH
#define SCENE_SCENE_HH

#include "Core/Renderer/CameraController/CameraController.hh"
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
    void destroy_entity(uint32_t id);

    inline Node& get_root() { return *m_root_node; }

    /// @brief Returns a view for the given component.
    /// @tparam ...Args Type of component used to construct the view.
    /// @return View for the given component.
    template<typename... Include, typename... Exclude>
    auto get_view(entt::exclude_t<Exclude...> exclude = entt::exclude_t<Exclude...>())
    {
      return m_registry.view<Include...>(exclude).each();
    }
    /// @brief Returns component of given entity.
    /// @param id Entity's id
    /// @tparam T Component to get.
    /// @return Component of given entity.
    template<typename T> T& get_component(uint32_t id) { return m_registry.get<T>((entt::entity)id); }

   private:
    Scene() : m_root_node{ Node::create_root(this) } {}

    friend class Entity;
  };
} // namespace esp

#endif // SCENE_SCENE_HH
