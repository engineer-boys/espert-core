#ifndef COMPONENTS_TAGCOMPONENT_H_
#define COMPONENTS_TAGCOMPONENT_H_

// std
#include <string>

namespace esp
{
	struct TagComponent
	{
		using TAG = std::string;

		TAG m_tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(TAG tag) : m_tag{ tag} {}
	};
}

#endif //COMPONENTS_TAGCOMPONENT_H_
