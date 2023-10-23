#include "GLFWContext.hh"

namespace esp
{
  bool GLFWContext::s_is_exist = false;

  GLFWContext::GLFWContext()
  {
    ESP_ASSERT(GLFWContext::s_is_exist == false,
               "GLFW context already exists.");

    int success = glfwInit();
    if (success == GLFW_FALSE)
    {
      ESP_CORE_ERROR("GLFW cannot be init.");
      throw std::runtime_error("GLFW cannot be init.");
    }

    GLFWContext::s_is_exist = true;
  }

  GLFWContext::~GLFWContext()
  {
    ESP_ASSERT(GLFWContext::s_is_exist == true,
               "GLFW context is empty and you are trying to delete it.");
    glfwTerminate();
    GLFWContext::s_is_exist = false;
  }

  std::unique_ptr<GLFWContext> GLFWContext::create()
  {
    return std::unique_ptr<GLFWContext>{ new GLFWContext() };
  }
} // namespace esp
