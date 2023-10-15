#ifndef RENDERER_RENDERER_H_
#define RENDERER_RENDERER_H_

#include "RenderAPI/EspRenderScheduler.hh"
#include "Scene/Components/Components.hh"
#include "Systems/Render/EspRenderSystem.hh"

namespace esp
{
	class EspRenderer
	{
	 private:
		//TODO: create scene
		// Scene _m_scene{};
		//TODO: scene - unordered_map[key: bit mask of components, value: list<entities>]
		static EspRenderer* s_instance;
		static std::unordered_map<TagComponent::TAG, EspRenderSystem&> render_systems;

		EspDevice m_device;
		std::unique_ptr<EspRenderScheduler> m_render_scheduler;


		EspRenderer(EspWindow& window);

	 public:
		~EspRenderer();
		static std::unique_ptr<EspRenderer> create(EspWindow& window);

		inline static EspRenderer* get_instance() { return s_instance; }

		static void add_render_system(TagComponent::TAG tag, EspRenderSystem& system);

		void on_window_resized();
	};
}

#endif //RENDERER_RENDERER_H_
