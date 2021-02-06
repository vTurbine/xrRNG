#ifndef VK_CONSOLE_H_
#define VK_CONSOLE_H_


namespace xrrng
{

struct RenderParams
{
    float f_ssa_discard{ 0.f }; // TODO: currently no discard at all
};

extern RenderParams params;

/**
 *
 */
void InitConsoleVK(void);

}

#endif // VK_CONSOLE_H_
