#ifndef CORE_RENDER_API_ESP_PRODUCT_PLAN_HH
#define CORE_RENDER_API_ESP_PRODUCT_PLAN_HH

#include "esppch.hh"

#include "Core/RenderAPI/RenderPlans/Block/EspBlock.hh"
#include "Core/RenderAPI/RenderPlans/Block/EspDepthBlock.hh"
#include "Core/RenderAPI/RenderPlans/Block/Types/EspImageLayout.hh"
#include "Core/RenderAPI/RenderPlans/Block/Types/EspSampleCountFlag.hh"
#include "Core/RenderAPI/RenderPlans/EspCommandBuffer.hh"

namespace esp
{
  class EspRenderPlan
  {
    /* -------------------------- METHODS ---------------------------------- */
   protected:
    EspImageLayout m_new_layout;

   public:
    EspRenderPlan() : m_new_layout{ EspImageLayout::ESP_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } {}
    virtual ~EspRenderPlan() {}

    virtual void add_block(std::shared_ptr<EspBlock> block)                  = 0;
    virtual void add_depth_block(std::shared_ptr<EspDepthBlock> depth_block) = 0;

    inline virtual void set_new_layout(EspImageLayout new_layout) { m_new_layout = new_layout; }

    virtual void set_command_buffer(EspCommandBufferId* id) = 0;
    virtual void build()                                    = 0;

    virtual void begin_plan() = 0;
    virtual void end_plan()   = 0;

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspRenderPlan> create();
    static std::unique_ptr<EspRenderPlan> create_final(
        glm::vec3 clear_color                = { 0.1f, 0.1f, 0.1f },
        EspSampleCountFlag sample_count_flag = EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT);
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_PRODUCT_PLAN_HH */
