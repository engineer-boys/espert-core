#ifndef ESPERT_ESPPCH_HH
#define ESPERT_ESPPCH_HH

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <utility>

// data types
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// glm
#include "glm/gtx/hash.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// volk
#include "volk.h"

// espert
#include "Core/Core.hh"
#include "Core/Logger.hh"

// namespaces
namespace fs = std::filesystem;

#endif // ESPERT_ESPPCH_HH