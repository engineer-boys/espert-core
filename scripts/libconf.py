import sys
from common import get_wsi_type, WSI, CmakeParameter


def should_use_wayland() -> bool:
    if not sys.platform.startswith("linux"):
        return False

    return get_wsi_type() == WSI.WAYLAND


ESPERT_LIBS_CONF = {
    "catch2": [],
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
    "glm": [],
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
    "tinyobjloader": [],
    "volk": [],
    "vulkan": [],
    "vvl": [
        CmakeParameter("UPDATE_DEPS", "ON"),
        CmakeParameter("BUILD_WERROR", "ON"),
        CmakeParameter("BUILD_TESTS", "OFF"),
    ],
}
