#ifndef SCENE_ENTITY_HH
#define SCENE_ENTITY_HH

#include "Scene.hh"

#include "esppch.hh"

namespace esp
{
  /// @brief Core part of ECS.
  class Entity
  {
   private:
    entt::entity m_handle{ entt::null };
    Scene* m_scene = nullptr;

   public:
    /// @brief Adds component to an Entity.
    /// @tparam T Type of component.
    /// @tparam ...Args Types of component args.
    /// @param ...args Component args.
    /// @return Added component.
    template<typename T, typename... Args> T& add_component(Args&&... args)
    {
      ESP_ASSERT(!has_component<T>(), "Entity already has component")
      return m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
    }

    /// @brief Returns reference to a certain type of component.
    /// @tparam T Type of component to return.
    /// @return Reference to component.
    template<typename T> T& get_component()
    {
      ESP_ASSERT(has_component<T>(), "Entity does not have component")
      return m_scene->m_registry.get<T>(m_handle);
    }

    /// @brief Returns pointer to a certain type of component if it exists.
    /// @tparam T Type of component to return.
    /// @return Pointer to component if exists. Else nullptr.
    template<typename T> T* try_get_component() { return m_scene->m_registry.try_get<T>(m_handle); }

    /// @brief Removes component of certain type from Entity.
    /// @tparam T Type of component to remove.
    template<typename T> void remove_component()
    {
      ESP_ASSERT(has_component<T>(), "Entity does not have component")
      m_scene->m_registry.remove<T>(m_handle);
    }

    /// @brief Checks for presence of certain component in an Entity.
    /// @tparam T Type of component to check for.
    /// @return True if Entity posesses component of said type. False otherwise.
    template<typename T> bool has_component() { return m_scene->m_registry.try_get<T>(m_handle); }

    inline entt::entity get_id() { return m_handle; }

   private:
    Entity(entt::entity handle, Scene* scene) : m_handle{ handle }, m_scene{ scene } {}

    friend class Scene;
  };
} // namespace esp

#endif // SCENE_ENTITY_HH
