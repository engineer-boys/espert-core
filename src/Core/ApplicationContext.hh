#ifndef ESPERT_CORE_APPLICATIONCONTEXT_HH
#define ESPERT_CORE_APPLICATIONCONTEXT_HH

#include "esppch.hh"

#include "Core/Systems/ResourceSystem.hh"
#include "Core/Systems/TextureSystem.hh"

namespace esp
{
  class ApplicationContext
  {
   private:
    static bool s_is_exist;

   private:
    std::unique_ptr<Logger> m_logger;
    std::unique_ptr<ResourceSystem> m_resource_system;
    std::unique_ptr<TextureSystem> m_texture_system;

    ApplicationContext();

   public:
    ~ApplicationContext();

    static std::unique_ptr<ApplicationContext> create();
  };
} // namespace esp

#endif // ESPERT_CORE_APPLICATIONCONTEXT_HH
