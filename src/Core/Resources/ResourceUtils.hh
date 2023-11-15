#ifndef ESPERT_CORE_RESOURCES_RESOURCEUTILS_HH
#define ESPERT_CORE_RESOURCES_RESOURCEUTILS_HH

#define VOID_DELETER                     \
  [](void* data)                         \
  {                                      \
    if (data != nullptr) { free(data); } \
  }

#define VOID_DELETER_TYPE void (*)(void*)

using resource_data_t = std::unique_ptr<void, VOID_DELETER_TYPE>;

#endif // ESPERT_CORE_RESOURCES_RESOURCEUTILS_HH