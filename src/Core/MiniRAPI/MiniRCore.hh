#ifndef ESPERT_CORE_MINIRAPI_MINIRLOADER_HH_
#define ESPERT_CORE_MINIRAPI_MINIRLOADER_HH_

#if defined(OPENGL_PLATFORM)
#error Not implemented
#elif defined(VULKAN_PLATFORM)
#include "volk.h"
#else
#error Unfortunatelly, neither Vulkan nor OpenGL is supported.
#endif

#endif /* ESPERT_CORE_MINIRAPI_MINIRLOADER_HH_ */
