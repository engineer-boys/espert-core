#ifndef RENDERER_RENDERER_H_
#define RENDERER_RENDERER_H_

#include "RenderAPI/EspRenderScheduler.hh"

namespace esp
{
	class Renderer
	{
	 private:
		//TODO: create scene
		// Scene _m_scene{};
		//TODO: scene - unordered_map[key: bit mask of components, value: list<entities>]
		static Renderer* s_instance;

		EspDevice m_device;
		EspRenderScheduler m_render_scheduler;

		Renderer(EspWindow& window);

	 public:
		~Renderer();
		static std::unique_ptr<Renderer> create(EspWindow& window);

		inline static Renderer* get_instance() { return Renderer::s_instance; }
	};
}

#endif //RENDERER_RENDERER_H_
