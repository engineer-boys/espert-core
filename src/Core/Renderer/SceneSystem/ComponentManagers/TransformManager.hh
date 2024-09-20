#ifndef ESPERT_SANDBOX_TRANSFORMMANAGER_HH
#define ESPERT_SANDBOX_TRANSFORMMANAGER_HH

#include "esppch.hh"

#include "Core/Renderer/SceneSystem/Action.hh"
#include "Core/Renderer/SceneSystem/Scene.hh"

namespace esp
{
  struct TransformComponent;

  class TransformManager
  {
   public:
    static void set_scene(Scene* scene);

    // ---------- version for entt id ----------
    static glm::mat4 get_model_mat(entt::entity entity, action::ActionType type = action::ESP_ABSOLUTE);
    static glm::vec3 get_translation(entt::entity entity, action::ActionType type = action::ESP_ABSOLUTE);
    static glm::quat get_rotation(entt::entity entity, action::ActionType type = action::ESP_ABSOLUTE);
    static glm::vec3 get_scale(entt::entity entity, action::ActionType type = action::ESP_ABSOLUTE);
    static TransformComponent& get_transform(entt::entity entity);

    static void translate(entt::entity entity, glm::vec3 vec);
    static void rotate(entt::entity entity, float angle, glm::vec3 axis);
    static void rotate(entt::entity entity, glm::quat quat);
    static void scale(entt::entity entity, float val);
    static void scale(entt::entity entity, glm::vec3 val);
    static void scale_ox(entt::entity entity, float val);
    static void scale_oy(entt::entity entity, float val);
    static void scale_oz(entt::entity entity, float val);

    static void set_translation(entt::entity entity, glm::vec3 vec, action::ActionType type = action::ESP_ABSOLUTE);
    static void set_rotation(entt::entity entity, float angle, glm::vec3 axis);
    static void set_rotation(entt::entity entity, glm::quat quat);
    static void set_scale(entt::entity entity, glm::vec3 val);

    // ---------- version for uint32_t id ----------
    static glm::mat4 get_model_mat(uint32_t entity, action::ActionType type = action::ESP_ABSOLUTE);
    static glm::vec3 get_translation(uint32_t entity, action::ActionType type = action::ESP_ABSOLUTE);
    static glm::quat get_rotation(uint32_t entity, action::ActionType type = action::ESP_ABSOLUTE);
    static glm::vec3 get_scale(uint32_t entity, action::ActionType type = action::ESP_ABSOLUTE);
    static TransformComponent& get_transform(uint32_t entity);

    static void translate(uint32_t entity, glm::vec3 vec);
    static void rotate(uint32_t entity, float angle, glm::vec3 axis);
    static void rotate(uint32_t entity, glm::quat quat);
    static void scale(uint32_t entity, float val);
    static void scale(uint32_t entity, glm::vec3 val);
    static void scale_ox(uint32_t entity, float val);
    static void scale_oy(uint32_t entity, float val);
    static void scale_oz(uint32_t entity, float val);

    static void set_translation(uint32_t entity, glm::vec3 vec, action::ActionType type = action::ESP_ABSOLUTE);
    static void set_rotation(uint32_t entity, float angle, glm::vec3 axis);
    static void set_rotation(uint32_t entity, glm::quat quat);
    static void set_scale(uint32_t entity, glm::vec3 val);
  };
} // namespace esp

#endif // ESPERT_SANDBOX_TRANSFORMMANAGER_HH
