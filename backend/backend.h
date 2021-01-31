#ifndef BACKEND_BACKEND_H_
#define BACKEND_BACKEND_H_

namespace xrrng
{

class Backend
{
public:
    void OnDeviceCreate();
    void OnDeviceDestroy();

    void OnFrameBegin();
    void OnFrameEnd();

    size_t GetContextsNum() const;
    vk::CommandBuffer GetCommandBuffer();

private:
    std::vector<vk::UniqueCommandBuffer> draw_buffers_;
};

extern Backend backend;

};

#endif // BACKEND_BACKEND_H_
