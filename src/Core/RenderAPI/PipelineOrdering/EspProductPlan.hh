#ifndef CORE_RENDER_API_ESP_PRODUCT_PLAN_HH
#define CORE_RENDER_API_ESP_PRODUCT_PLAN_HH

#include "esppch.hh"

#include "Core/RenderAPI/PipelineOrdering/Block/EspBlock.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/EspDepthBlock.hh"

namespace esp
{
  class EspProductPlan
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    virtual ~EspProductPlan() {}

    virtual void add_block(std::shared_ptr<EspBlock> block)                  = 0;
    virtual void add_depth_block(std::shared_ptr<EspDepthBlock> depth_block) = 0;

    virtual void begin_plan() = 0;

    virtual void end_plan() = 0;

    /* -------------------------- METHODS STATIC --------------------------- */
   public:
    static std::unique_ptr<EspProductPlan> build();
    static std::unique_ptr<EspProductPlan> build_final();
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_PRODUCT_PLAN_HH */
