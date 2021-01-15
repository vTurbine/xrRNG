#ifndef FACTORY_WALLMARK_ARRAY_H_
#define FACTORY_WALLMARK_ARRAY_H_

#include <Include/xrRender/WallMarkArray.h>


class fWallMarkArray
    : public IWallMarkArray
{
public:
    fWallMarkArray() = default;
    ~fWallMarkArray() = default;

    void Copy(IWallMarkArray& obj) override;
    void AppendMark(LPCSTR textures) override;
    void clear() override;
    bool empty() override;
    wm_shader GenerateWallmark() override;
};

#endif // FACTORY_WALLMARK_ARRAY_H_
