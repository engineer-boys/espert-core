#ifndef ESPERT_CORESYSTEMS_ESPERTWINDOW_HH_
#define ESPERT_CORESYSTEMS_ESPERTWINDOW_HH_

#include "esppch.hh"
#include "CoreSystems/Events/Event.hh"
#include "GLFW/glfw3.h"

namespace Espert
{
    class EspertWindow
    {
    public:
        struct WindowData
        {
        public:
            using EventManagerFun = std::function<void(Event &)>;

        public:
            std::string m_title;
            unsigned int m_width;
            unsigned int m_height;
            EventManagerFun m_events_manager_fun;

            WindowData(
                const std::string title = "Espert widnow",
                unsigned int width = 1280,
                unsigned int height = 720)
                : m_title(title), m_width(width), m_height(height) {}
        };

    private:
        static bool _s_is_exist;

    private:
        WindowData _m_data;
        GLFWwindow *_m_window;

    private:
        void init(const WindowData &data);
        void destroy();
		void set_callbacks();

        EspertWindow(const WindowData &data);

    public:
        ~EspertWindow();

        void update();

        inline unsigned int get_width() { return _m_data.m_width; }
        inline unsigned int get_height() { return _m_data.m_height; }
        inline void set_events_manager_fun(const WindowData::EventManagerFun &callback) { _m_data.m_events_manager_fun = callback; }

        static std::unique_ptr<EspertWindow> create(const WindowData &data);
    };
} // namespace Espert

#endif  // ESPERT_CORESYSTEMS_ESPERTWINDOW_HH_
