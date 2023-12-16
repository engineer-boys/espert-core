#ifndef SCENE_ENTITY_HH
#define SCENE_ENTITY_HH

#include "Scene.hh"

#include "esppch.hh"

namespace esp
{
  class Entity
  {
   private:
    entt::entity m_handle{ 0 };
    Scene* m_scene;

   public:
    template<typename T, typename... Args> T& add_component(Args&&... args)
    {
      ESP_ASSERT(!has_component<T>(), "Entity already has component")
      return m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T> T& get_component()
    {
      ESP_ASSERT(has_component<T>(), "Entity does not have component")
      return m_scene->m_registry.get<T>(m_handle);
    }

    template<typename T> void remove_component()
    {
      ESP_ASSERT(has_component<T>(), "Entity does not have component")
      m_scene->m_registry.remove<T>(m_handle);
    }

    template<typename T> bool has_component() { return m_scene->m_registry.try_get<T>(m_handle); }

   private:
    Entity(entt::entity handle, Scene* scene) : m_handle{ handle }, m_scene{ scene } {}

    friend class Scene;
  };
} // namespace esp

#endif // SCENE_ENTITY_HH
