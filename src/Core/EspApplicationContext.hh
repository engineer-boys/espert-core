#ifndef ESPERT_CORE_APPLICATIONCONTEXT_HH
#define ESPERT_CORE_APPLICATIONCONTEXT_HH

#include "esppch.hh"

namespace esp
{
  /// @brief Context of the application
  class EspApplicationContext
  {
   private:
    static EspApplicationContext* s_instance;

   private:
    std::unique_ptr<Logger> m_logger;

    EspApplicationContext();

   public:
    /// @brief Destructor removes the instance of application context.
    ~EspApplicationContext();

    /// @brief Creates new instance of application context.
    /// @return Unique pointer to instance of application context.
    static std::unique_ptr<EspApplicationContext> create();
  };
} // namespace esp

#endif // ESPERT_CORE_APPLICATIONCONTEXT_HH
