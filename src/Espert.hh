#ifndef ESPERT_ESPERT_HH
#define ESPERT_ESPERT_HH

/* In this header file are all needed headers */
/* ------------------------------------------ */
#include "Core/Utils.hh"

#include "Core/Application.hh"
#include "Core/Logger.hh"

#include "Core/Events/Events.hh"
#include "Core/Layers/Layer.hh"

/* --------- Render API --------------------- */
#include "Core/RenderAPI/EspCommandHandler.hh"
#include "Core/RenderAPI/Pipeline/EspPipeline.hh"
#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"
#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"
#include "Core/RenderAPI/Uniforms/EspUniformMetaData.hh"

/* --------- Scene --------------------- */
#include "Core/Scene/Action.hh"
#include "Core/Scene/Components/Components.hh"
#include "Core/Scene/Entity.hh"
#include "Core/Scene/Model.hh"
#include "Core/Scene/Node.hh"
#include "Core/Scene/Scene.hh"

/* --------- Renderer --------------------- */
#include "Core/Renderer/Camera.hh"

/* --------- Client Point ------------------- */
#include "Core/ClientPoint.hh"

/* ------------------------------------------ */

#endif // ESPERT_ESPERT_HH
