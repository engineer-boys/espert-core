#ifndef __ENGINE__CLIENT_POINT_HH__
#define __ENGINE__CLIENT_POINT_HH__

#include "Application.hh"

extern Esper::Application *Esper::createAppInstance();

int main(int argc, char **argv)
{
    Esper::Application *app_instance = Esper::createAppInstance();
    app_instance->run();
    delete app_instance;
}

#endif /* __ENGINE__CLIENT_POINT_HH__ */
