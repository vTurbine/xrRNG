#ifndef FACTORY_UI_SEQUENCE_VIDEO_ITEM_H_
#define FACTORY_UI_SEQUENCE_VIDEO_ITEM_H_

#include <Include/xrRender/UISequenceVideoItem.h>


class fUISequenceVideoItem
    : public IUISequenceVideoItem
{
public:
    fUISequenceVideoItem() = default;
    ~fUISequenceVideoItem() = default;

    // Interface implementation
    void Copy(IUISequenceVideoItem& obj) override;
    bool HasTexture() override;
    void CaptureTexture() override;
    void ResetTexture() override;
    BOOL video_IsPlaying() override;
    void video_Sync(u32 time) override;
    void video_Play(BOOL looped, u32 time = 0xFFFFFFFF) override;
    void video_Stop() override;
};

#endif // FACTORY_UI_SEQUENCE_VIDEO_ITEM_H_
