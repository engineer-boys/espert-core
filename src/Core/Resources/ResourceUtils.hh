#ifndef ESPERT_CORE_RESOURCES_RESOURCEUTILS_HH
#define ESPERT_CORE_RESOURCES_RESOURCEUTILS_HH

#define VOID_DELETER                     \
  [](void* data)                         \
  {                                      \
    if (data != nullptr) { free(data); } \
  }

#define VOID_DELETER_TYPE void (*)(void*)

namespace esp
{
  using resource_data_t = std::unique_ptr<void, VOID_DELETER_TYPE>;

  template<class OutType, class InType> std::unique_ptr<OutType> unique_cast(std::unique_ptr<InType> ptr)
  {
    return std::unique_ptr<OutType>(dynamic_cast<OutType*>(ptr.release()));
  }
} // namespace esp

#endif // ESPERT_CORE_RESOURCES_RESOURCEUTILS_HH