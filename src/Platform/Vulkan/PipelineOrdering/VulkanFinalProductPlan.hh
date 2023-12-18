#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH

// libs
#include "esppch.hh"

// Render API Vulkan
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanBlock.hh"
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanDepthBlock.hh"

// Render API
#include "Core/RenderAPI/PipelineOrdering/EspProductPlan.hh"

// std
#include <vector>

namespace esp
{
  class VulkanFinalProductPlan : public EspProductPlan
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    std::shared_ptr<VulkanDepthBlock> m_depth_block = nullptr;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanFinalProductPlan();
    virtual ~VulkanFinalProductPlan();

    VulkanFinalProductPlan(const VulkanFinalProductPlan& other)            = delete;
    VulkanFinalProductPlan& operator=(const VulkanFinalProductPlan& other) = delete;

    virtual void add_block(std::shared_ptr<EspBlock> block) override {}
    virtual void add_depth_block(std::shared_ptr<EspDepthBlock> depth_block) override;

    virtual void begin_plan() override;
    virtual void end_plan() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH */