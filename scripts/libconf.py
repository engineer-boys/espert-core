import sys
from common import CmakeParameter


ESPERT_LIBS_CONF = {
    "assimp": [
        CmakeParameter("BUILD_SHARED_LIBS", "OFF"),
        CmakeParameter("ASSIMP_INSTALL_PDB", "OFF"),
    ],
    "catch2": [],
    "doxygen": [],
    "entt": [],
    "glfw": [
        CmakeParameter("GLFW_BUILD_DOCS", "OFF"),
        CmakeParameter("GLFW_BUILD_TESTS", "OFF"),
        CmakeParameter("GLFW_BUILD_EXAMPLES", "OFF"),
        CmakeParameter("GLFW_BUILD_TESTS", "OFF"),
        CmakeParameter("GLFW_INSTALL", "ON"),
        CmakeParameter("BUILD_SHARED_LIBS", "OFF"),
        CmakeParameter("GLFW_VULKAN_STATIC", "ON"),
    ],
    "glm": [
        CmakeParameter("BUILD_TESTING", "OFF"),
    ],
    "glslang": [
        CmakeParameter("ENABLE_SPVREMAPPER", "OFF"),
        CmakeParameter("ENABLE_GLSLANG_BINARIES", "ON"),
        CmakeParameter("ENABLE_HLSL", "OFF"),
        CmakeParameter("ENABLE_OPT", "OFF"),
        CmakeParameter("ENABLE_CTEST", "OFF"),
        CmakeParameter("BUILD_TESTING", "OFF"),
        CmakeParameter("BUILD_EXTERNAL", "OFF"),
    ],
    "spdlog": [
        CmakeParameter("SPDLOG_ENABLE_PCH", "ON"),
        CmakeParameter("SPDLOG_BUILD_EXAMPLE", "OFF"),
    ],
    "stb": [],
    "tinyobjloader": [],
    "volk": [],
    "vulkan": [],
    "vvl": [
        CmakeParameter("UPDATE_DEPS", "ON"),
        CmakeParameter("BUILD_WERROR", "ON"),
        CmakeParameter("BUILD_TESTS", "OFF"),
    ]
}
