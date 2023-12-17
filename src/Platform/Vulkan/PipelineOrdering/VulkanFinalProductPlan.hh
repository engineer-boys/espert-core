#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH

// libs
#include "esppch.hh"

// Render API Vulkan
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanBlock.hh"

// Render API
#include "Core/RenderAPI/PipelineOrdering/EspProductPlan.hh"

// std
#include <vector>

namespace esp
{
  class VulkanFinalProductPlan : public EspProductPlan
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanFinalProductPlan();
    virtual ~VulkanFinalProductPlan();

    VulkanFinalProductPlan(const VulkanFinalProductPlan& other)            = delete;
    VulkanFinalProductPlan& operator=(const VulkanFinalProductPlan& other) = delete;

    virtual void add_building_block(std::shared_ptr<EspBlock> block) override {}
    virtual void begin_plan() override;
    virtual void end_plan() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH */