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
#include "Core/RenderAPI/RenderPlans/EspCommandBuffer.hh"
#include "Core/RenderAPI/RenderPlans/EspRenderPlan.hh"
#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"
#include "Core/RenderAPI/Uniforms/EspUniformMetaData.hh"
#include "Core/RenderAPI/Worker/EspWorker.hh"
#include "Core/RenderAPI/Worker/EspWorkerBuilder.hh"

/* --------- Renderer --------------------- */
#include "Core/Renderer/Model/Animation/Animator.hh"
#include "Core/Renderer/Model/Model.hh"

#include "Core/Renderer/Camera/FpsCamera.hh"
#include "Core/Renderer/Camera/OrbitCamera.hh"
#include "Core/Renderer/CameraController/FpsCameraController.hh"
#include "Core/Renderer/CameraController/OrbitCameraController.hh"
#include "Core/Renderer/SceneSystem/Components/Components.hh"
#include "Core/Renderer/SceneSystem/Entity.hh"
#include "Core/Renderer/SceneSystem/Node.hh"
#include "Core/Renderer/SceneSystem/Scene.hh"

/* ------------------------------------------ */

#endif // ESPERT_ESPERT_HH
