#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH

// libs
#include "esppch.hh"

// Render API
#include "Core/RenderAPI/PipelineOrdering/EspProductPlan.hh"

// Render API Vulkan
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanBlock.hh"

// std
#include <vector>

namespace esp
{
  class VulkanProductPlan : public EspProductPlan
  {
   private:
    std::vector<std::shared_ptr<VulkanBlock>> m_blocks;

   public:
    VulkanProductPlan();
    virtual ~VulkanProductPlan();

    VulkanProductPlan(const VulkanProductPlan& other)            = delete;
    VulkanProductPlan& operator=(const VulkanProductPlan& other) = delete;

    virtual void add_building_block(std::shared_ptr<EspBlock> block) override;
    virtual void begin_plan() override;
    virtual void end_plan() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH */