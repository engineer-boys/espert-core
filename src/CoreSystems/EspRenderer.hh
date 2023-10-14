#ifndef RENDERER_RENDERER_H_
#define RENDERER_RENDERER_H_

#include "RenderAPI/EspRenderScheduler.hh"

namespace esp
{
	class EspRenderer
	{
	 private:
		//TODO: create scene
		// Scene _m_scene{};
		//TODO: scene - unordered_map[key: bit mask of components, value: list<entities>]
		static EspRenderer* s_instance;

		EspDevice m_device;
		std::unique_ptr<EspRenderScheduler> m_render_scheduler;

		EspRenderer(EspWindow& window);

	 public:
		~EspRenderer();
		static std::unique_ptr<EspRenderer> create(EspWindow& window);

		inline static EspRenderer* get_instance() { return s_instance; }

		void on_window_resized();
	};
}

#endif //RENDERER_RENDERER_H_
