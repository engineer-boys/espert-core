#ifndef ESPERT_ESPPCH_HH
#define ESPERT_ESPPCH_HH

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <utility>

// data types
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// glm
#include "glm/gtx/hash.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// volk
#include "volk.h"

// entt
#include "entt/entt.hpp"

// espert
#include "Core/Utils/Logger.hh"
#include "Core/Utils/Utils.hh"

// namespaces
namespace fs = std::filesystem;

#endif // ESPERT_ESPPCH_HH