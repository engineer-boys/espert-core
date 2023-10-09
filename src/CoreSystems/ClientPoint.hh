#ifndef __ENGINE__CLIENT_POINT_HH__
#define __ENGINE__CLIENT_POINT_HH__

#include "Application.hh"
#include "ContextApplication.hh"

extern Esper::Application *Esper::createAppInstance();

int main(int argc, char **argv)
{
    auto context = Esper::ContextApplication::create();
    Esper::Application *app_instance = Esper::createAppInstance();

    /* running */
    {
        app_instance->set_context(std::move(context));
        app_instance->run();
    }

    delete app_instance;
}

#endif /* __ENGINE__CLIENT_POINT_HH__ */
