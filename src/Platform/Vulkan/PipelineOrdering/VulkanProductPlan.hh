#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH

// libs
#include "esppch.hh"

// Render API
#include "Core/RenderAPI/PipelineOrdering/EspProductPlan.hh"
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanDepthBlock.hh"

// Render API Vulkan
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanBlock.hh"

// std
#include <vector>

namespace esp
{
  class VulkanProductPlan : public EspProductPlan
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    std::vector<std::shared_ptr<VulkanBlock>> m_blocks;
    std::shared_ptr<VulkanDepthBlock> m_depth_block = nullptr;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanProductPlan();
    virtual ~VulkanProductPlan();

    VulkanProductPlan(const VulkanProductPlan& other)            = delete;
    VulkanProductPlan& operator=(const VulkanProductPlan& other) = delete;

    virtual void add_block(std::shared_ptr<EspBlock> block) override;
    virtual void add_depth_block(std::shared_ptr<EspDepthBlock> depth_block) override;

    virtual void begin_plan() override;
    virtual void end_plan() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH */