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

  /// @brief Casts unique pointer of base class (InType) to specific class (OutType).
  /// @tparam OutType Specific class type.
  /// @tparam InType Base class type (can be omitted).
  /// @param ptr Unique pointer to object of base class.
  /// @return Unique pointer to object of specific class.
  template<class OutType, class InType> std::unique_ptr<OutType> unique_cast(std::unique_ptr<InType> ptr)
  {
    return std::unique_ptr<OutType>(dynamic_cast<OutType*>(ptr.release()));
  }
} // namespace esp

#endif // ESPERT_CORE_RESOURCES_RESOURCEUTILS_HH