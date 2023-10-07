#ifndef __ENGINE__APPLICATION_HH__
#define __ENGINE__APPLICATION_HH__

namespace Esper
{
    class Application
    {
    private:
    public:
        Application();
        virtual ~Application();

        void run();
    };

    /* This function is defined by CLIENT */
    Application *createAppInstance();

} // namespace  Esper

#endif /* __ENGINE__APPLICATION_HH__ */
