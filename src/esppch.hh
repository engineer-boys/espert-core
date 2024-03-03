#ifndef ESPERT_ESPPCH_HH
#define ESPERT_ESPPCH_HH

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <utility>
#include <variant>

// data types
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// glm
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/hash.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// volk
#include "volk.h"

// entt
#include "entt/entt.hpp"

// imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// espert
#include "Core/Utils/Logger.hh"
#include "Core/Utils/Utils.hh"

// namespaces
namespace fs = std::filesystem;

#endif // ESPERT_ESPPCH_HH