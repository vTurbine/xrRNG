#ifndef FACTORY_CONSOLE_RENDER_H_
#define FACTORY_CONSOLE_RENDER_H_

#include <Include/xrRender/ConsoleRender.h>


class fConsoleRender
    : public IConsoleRender
{
public:
    fConsoleRender() = default;
    ~fConsoleRender() = default;

    void Copy(IConsoleRender& obj) override;
    void OnRender(bool inGame) override;
};

#endif // FACTORY_CONSOLE_RENDER_H_
