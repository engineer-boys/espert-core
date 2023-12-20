#ifndef ESPERT_CORE_APPLICATIONCONTEXT_HH
#define ESPERT_CORE_APPLICATIONCONTEXT_HH

#include "esppch.hh"

namespace esp
{
  class EspApplicationContext
  {
   private:
    static EspApplicationContext* s_instance;

   private:
    std::unique_ptr<Logger> m_logger;

    EspApplicationContext();

   public:
    ~EspApplicationContext();

    static std::unique_ptr<EspApplicationContext> create();
  };
} // namespace esp

#endif // ESPERT_CORE_APPLICATIONCONTEXT_HH
