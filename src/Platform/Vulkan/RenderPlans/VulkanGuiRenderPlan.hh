#ifndef ESPERT_SANDBOX_VULKANGUIRENDERPLAN_HH
#define ESPERT_SANDBOX_VULKANGUIRENDERPLAN_HH

// Render API
#include "Core/RenderAPI/RenderPlans/EspGuiRenderPlan.hh"

namespace esp
{
  class VulkanGuiRenderPlan : public EspGuiRenderPlan
  {
   private:
    uint32_t m_height;
    uint32_t m_width;
    
   public:
    VulkanGuiRenderPlan()  = default;
    ~VulkanGuiRenderPlan() = default;

    PREVENT_COPY(VulkanGuiRenderPlan);

    virtual void build() override;

    virtual void begin_plan(EspRenderArea render_area) override;
    virtual void end_plan() override;
  };
} // namespace esp

#endif // ESPERT_SANDBOX_VULKANGUIRENDERPLAN_HH
