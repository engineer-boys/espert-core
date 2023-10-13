#ifndef ESPERT_CORESYSTEMS_CORE_HH_
#define ESPERT_CORESYSTEMS_CORE_HH_

#define ESP_BIND_EVENT_FOR_FUN(obj) std::bind(&obj, this, std::placeholders::_1)

#endif  // ESPERT_CORESYSTEMS_CORE_HH_
