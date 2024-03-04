#ifndef ESPERT_SANDBOX_VULKANGUI_HH
#define ESPERT_SANDBOX_VULKANGUI_HH

#include "Core/EspGui.hh"

namespace esp
{
  class VulkanGui : public EspGui
  {
   private:
    static constexpr ImVec2 OFFSET{ 10, 10 };

    static VulkanGui* s_instance;

    VkDescriptorPool m_desc_pool;

   public:
    static std::unique_ptr<VulkanGui> create();

    PREVENT_COPY(VulkanGui)
    ~VulkanGui() = default;

    static void terminate();
    static void new_frame();
    static void end_frame();
    static void render();

   private:
    VulkanGui() = default;

    void init();
  };
} // namespace esp

#endif // ESPERT_SANDBOX_VULKANGUI_HH
