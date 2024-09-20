#ifndef ESPERT_SANDBOX_NODEMANAGER_HH
#define ESPERT_SANDBOX_NODEMANAGER_HH

#include "esppch.hh"

#include "Core/Renderer/SceneSystem/Scene.hh"

namespace esp
{
  struct NodeComponent;

  class NodeManager
  {
   public:
    static void set_scene(Scene* scene);

    static NodeComponent& get_node(entt::entity id);
    static NodeComponent& get_node(uint32_t id);
  };
} // namespace esp

#endif // ESPERT_SANDBOX_NODEMANAGER_HH
