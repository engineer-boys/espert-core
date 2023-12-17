#ifndef ESPERT_CORE_APPLICATIONCONTEXT_HH
#define ESPERT_CORE_APPLICATIONCONTEXT_HH

#include "esppch.hh"

namespace esp
{
  class ApplicationContext
  {
   private:
    static ApplicationContext* s_instance;

   private:
    std::unique_ptr<Logger> m_logger;

    ApplicationContext();

   public:
    ~ApplicationContext();

    static std::unique_ptr<ApplicationContext> create();
  };
} // namespace esp

#endif // ESPERT_CORE_APPLICATIONCONTEXT_HH
