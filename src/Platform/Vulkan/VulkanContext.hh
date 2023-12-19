#ifndef VULKAN_RENDER_API_VULKAN_CONTEXT_HH
#define VULKAN_RENDER_API_VULKAN_CONTEXT_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/EspRenderContext.hh"

// Render API Vulkan
#include "VulkanContextData.hh"
#include "VulkanDevice.hh"
#include "VulkanResourceManager.hh"

// std
#include <optional>

namespace esp
{
  class VulkanContext : public EspRenderContext
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    static VulkanContext* s_instance;

    VulkanContextData m_context_data;

    std::unique_ptr<VulkanDevice> m_vulkan_device{};
    std::unique_ptr<VulkanResourceManager> m_vulkan_resource_manager{};

   private:
    void create_instance();
    void create_surface(EspWindow& window);
    void create_vulkan_device();
    void create_default_sampler();
    void create_vulkan_resource_manager();

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanContext();
    ~VulkanContext() override = default;

    VulkanContext(const VulkanContext& other)            = delete;
    VulkanContext& operator=(const VulkanContext& other) = delete;

    void init(EspWindow& window) override;
    void terminate() override;

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<VulkanContext> create(EspWindow& window);

    inline static const VulkanContextData& get_context_data() { return s_instance->m_context_data; }
  };

} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_CONTEXT_HH
