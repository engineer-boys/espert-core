#ifndef ESPERT_ESPERT_HH
#define ESPERT_ESPERT_HH

/* In this header file are all needed headers */
/* ------------------------------------------ */
#include "Core/Utils/Utils.hh"

#include "Core/EspApplication.hh"
#include "Core/Utils/Logger.hh"

#include "Core/Events/Events.hh"
#include "Core/Layers/Layer.hh"

/* --------- Input --------------------- */
#include "Core/Input/EspInput.hh"
#include "Core/Input/EspKeyCodes.hh"
#include "Core/Input/EspMouseButtonCode.hh"

/* --------- Render API --------------------- */
#include "Core/RenderAPI/Pipeline/EspPipeline.hh"
#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"
#include "Core/RenderAPI/PipelineOrdering/EspRenderPlan.hh"
#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"
#include "Core/RenderAPI/Uniforms/EspUniformMetaData.hh"

/* --------- Renderer --------------------- */
#include "Core/Renderer/Camera.hh"
#include "Core/Renderer/Model/Model.hh"
#include "Core/Renderer/SceneSystem/Action.hh"
#include "Core/Renderer/SceneSystem/Components/Components.hh"
#include "Core/Renderer/SceneSystem/Entity.hh"
#include "Core/Renderer/SceneSystem/Node.hh"
#include "Core/Renderer/SceneSystem/Scene.hh"

/* --------- Client Point ------------------- */
#include "Core/ClientPoint.hh"

/* ------------------------------------------ */

#endif // ESPERT_ESPERT_HH
