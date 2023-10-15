#ifndef ESPERT_CORE_CORE_HH_
#define ESPERT_CORE_CORE_HH_

#define ESP_BIND_EVENT_FOR_FUN(obj) std::bind(&obj, this, std::placeholders::_1)

// std
#include <functional>

namespace esp
{
	// from: https://stackoverflow.com/a/57595105
	template<typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest& ... rest)
	{
		seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	}
}

#endif  // ESPERT_CORE_CORE_HH_
