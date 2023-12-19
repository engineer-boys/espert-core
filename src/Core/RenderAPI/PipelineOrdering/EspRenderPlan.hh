#ifndef CORE_RENDER_API_ESP_PRODUCT_PLAN_HH
#define CORE_RENDER_API_ESP_PRODUCT_PLAN_HH

#include "esppch.hh"

#include "Core/RenderAPI/PipelineOrdering/Block/EspBlock.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/EspDepthBlock.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspSampleCountFlag.hh"

namespace esp
{
  class EspRenderPlan
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    virtual ~EspRenderPlan() {}

    virtual void add_block(std::shared_ptr<EspBlock> block)                  = 0;
    virtual void add_depth_block(std::shared_ptr<EspDepthBlock> depth_block) = 0;

    virtual void begin_plan() = 0;

    virtual void end_plan() = 0;

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspRenderPlan> build();
    static std::unique_ptr<EspRenderPlan> build_final(
        EspSampleCountFlag sample_count_flag = EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT);
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_PRODUCT_PLAN_HH */
