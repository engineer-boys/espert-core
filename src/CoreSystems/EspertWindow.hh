#ifndef __ENGINE__WINDOW_HH__
#define __ENGINE__WINDOW_HH__

#include <memory>
#include <functional>
#include <string>

#include "Events/Event.hh"
#include "GLFW/glfw3.h"

namespace Esper
{
    class EsperWindow
    {
    public:
        struct WindowData
        {
        public:
            using EventManagerFun = std::function<void(Event &)>;

        public:
            std::string title;
            unsigned int width;
            unsigned int height;
            EventManagerFun event_manager_fun;

            WindowData(
                const std::string title = "Esper widnow",
                unsigned int width = 1280,
                unsigned int height = 720)
                : title(title), width(width), height(height) {}
        };

    private:
        static bool _s_is_exist;

    private:
        WindowData _m_data;
        GLFWwindow *_m_window;

    private:
        void init(const WindowData &data);
        void destroy();

        EsperWindow(const WindowData &data);

    public:
        ~EsperWindow();

        void on_update();

        inline unsigned int get_width() { return _m_data.width; }
        inline unsigned int get_height() { return _m_data.height; }
        inline void set_event_manager_fun(const WindowData::EventManagerFun &callback) { _m_data.event_manager_fun = callback; }

        static std::unique_ptr<EsperWindow> create(const WindowData &data);
    };
} // namespace Esper

#endif /* __ENGINE__WINDOW_HH__ */
