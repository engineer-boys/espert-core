#ifndef ESPERT_CORESYSTEMS_APPLICATIONCONTEXT_HH_
#define ESPERT_CORESYSTEMS_APPLICATIONCONTEXT_HH_

#include "esppch.hh"

namespace Espert {
class ApplicationContext {
  private:
  static bool _s_is_exist;

  private:
  std::unique_ptr<Logger> _m_logger;

  ApplicationContext();

  public:
  ~ApplicationContext();

  static std::unique_ptr<ApplicationContext> create();
};

} // namespace Espert

#endif // ESPERT_CORESYSTEMS_APPLICATIONCONTEXT_HH_
