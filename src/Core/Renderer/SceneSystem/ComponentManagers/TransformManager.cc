#include "TransformManager.hh"
#include "Core/Renderer/SceneSystem/Components/NodeComponent.hh"
#include "Core/Renderer/SceneSystem/Components/TransformComponent.hh"

using namespace esp::action;

namespace esp
{
  static Scene* s_scene;

  void TransformManager::set_scene(Scene* scene) { s_scene = scene; }

  glm::mat4 TransformManager::get_model_mat(entt::entity entity, action::ActionType type)
  {
    auto& transform = get_transform(entity);
    auto& parent    = s_scene->get_component<NodeComponent>(entity).m_parent;

    glm::mat4 model = transform.get_model_mat();
    if (type == ActionType::ESP_RELATIVE || parent == entt::null) { return model; }
    else { return get_model_mat(parent) * model; }
  }

  glm::vec3 TransformManager::get_translation(entt::entity entity, action::ActionType type)
  {
    auto& transform = get_transform(entity);
    auto& parent    = s_scene->get_component<NodeComponent>(entity).m_parent;

    glm::vec3 translation = transform.get_translation();
    if (type == ActionType::ESP_ABSOLUTE && parent != entt::null)
    {
      return translation + get_translation(parent, type);
    }
    return translation;
  }

  glm::quat TransformManager::get_rotation(entt::entity entity, action::ActionType type)
  {
    auto& transform = get_transform(entity);
    auto& parent    = s_scene->get_component<NodeComponent>(entity).m_parent;

    glm::quat rotation = transform.get_rotation();
    if (type == ActionType::ESP_ABSOLUTE && parent != entt::null) { return get_rotation(parent, type) * rotation; }
    return rotation;
  }

  glm::vec3 TransformManager::get_scale(entt::entity entity, action::ActionType type)
  {
    auto& transform = get_transform(entity);
    auto& parent    = s_scene->get_component<NodeComponent>(entity).m_parent;

    glm::vec3 scale = transform.get_scale();
    if (type == ActionType::ESP_ABSOLUTE && parent != entt::null) { return scale * get_scale(parent, type); }
    return scale;
  }

  TransformComponent& TransformManager::get_transform(entt::entity entity)
  {
    return s_scene->get_component<TransformComponent>(entity);
  }

  void TransformManager::translate(entt::entity entity, glm::vec3 vec)
  {
    auto& transform = get_transform(entity);
    transform.translate(vec);
  }

  void TransformManager::rotate(entt::entity entity, float angle, glm::vec3 axis)
  {
    auto& transform = get_transform(entity);
    transform.rotate(angle, axis);
  }

  void TransformManager::rotate(entt::entity entity, glm::quat quat)
  {
    auto& transform = get_transform(entity);
    transform.rotate(quat);
  }

  void TransformManager::scale(entt::entity entity, float val)
  {
    auto& transform = get_transform(entity);
    transform.scale(val);
  }

  void TransformManager::scale(entt::entity entity, glm::vec3 val)
  {
    auto& transform = get_transform(entity);
    transform.scale(val);
  }

  void TransformManager::scale_ox(entt::entity entity, float val)
  {
    auto& transform = get_transform(entity);
    transform.scale_ox(val);
  }

  void TransformManager::scale_oy(entt::entity entity, float val)
  {
    auto& transform = get_transform(entity);
    transform.scale_oy(val);
  }

  void TransformManager::scale_oz(entt::entity entity, float val)
  {
    auto& transform = get_transform(entity);
    transform.scale_oz(val);
  }

  void TransformManager::set_translation(entt::entity entity, glm::vec3 vec, action::ActionType type)
  {
    auto& transform = get_transform(entity);
    if (type == ActionType::ESP_RELATIVE)
    {
      auto& children = s_scene->get_component<NodeComponent>(entity).m_children;
      for (auto& child : children)
      {
        auto& child_transform = get_transform(child);
        child_transform.translate(transform.get_translation() - vec);
      }
    }
    transform.set_translation(vec);
  }

  void TransformManager::set_rotation(entt::entity entity, float angle, glm::vec3 axis)
  {
    auto& transform = get_transform(entity);
    transform.set_rotation(angle, axis);
  }

  void TransformManager::set_rotation(entt::entity entity, glm::quat quat)
  {
    auto& transform = get_transform(entity);
    transform.set_rotation(quat);
  }

  void TransformManager::set_scale(entt::entity entity, glm::vec3 val)
  {
    auto& transform = get_transform(entity);
    transform.set_scale(val);
  }

  glm::mat4 TransformManager::get_model_mat(uint32_t entity, action::ActionType type)
  {
    return get_model_mat((entt::entity)entity, type);
  }

  glm::vec3 TransformManager::get_translation(uint32_t entity, action::ActionType type)
  {
    return get_translation((entt::entity)entity, type);
  }

  glm::quat TransformManager::get_rotation(uint32_t entity, action::ActionType type)
  {
    return get_rotation((entt::entity)entity, type);
  }

  glm::vec3 TransformManager::get_scale(uint32_t entity, action::ActionType type)
  {
    return get_scale((entt::entity)entity, type);
  }

  TransformComponent& TransformManager::get_transform(uint32_t entity) { return get_transform((entt::entity)entity); }

  void TransformManager::translate(uint32_t entity, glm::vec3 vec) { return translate((entt::entity)entity, vec); }

  void TransformManager::rotate(uint32_t entity, float angle, glm::vec3 axis)
  {
    return rotate((entt::entity)entity, angle, axis);
  }

  void TransformManager::rotate(uint32_t entity, glm::quat quat) { return rotate((entt::entity)entity, quat); }

  void TransformManager::scale(uint32_t entity, float val) { return scale((entt::entity)entity, val); }

  void TransformManager::scale(uint32_t entity, glm::vec3 val) { return scale((entt::entity)entity, val); }

  void TransformManager::scale_ox(uint32_t entity, float val) { return scale_ox((entt::entity)entity, val); }

  void TransformManager::scale_oy(uint32_t entity, float val) { return scale_oy((entt::entity)entity, val); }

  void TransformManager::scale_oz(uint32_t entity, float val) { return scale_oz((entt::entity)entity, val); }

  void TransformManager::set_translation(uint32_t entity, glm::vec3 vec, action::ActionType type)
  {
    return set_translation((entt::entity)entity, vec, type);
  }

  void TransformManager::set_rotation(uint32_t entity, float angle, glm::vec3 axis)
  {
    return set_rotation((entt::entity)entity, angle, axis);
  }

  void TransformManager::set_rotation(uint32_t entity, glm::quat quat)
  {
    return set_rotation((entt::entity)entity, quat);
  }

  void TransformManager::set_scale(uint32_t entity, glm::vec3 val) { return set_scale((entt::entity)entity, val); }
} // namespace esp