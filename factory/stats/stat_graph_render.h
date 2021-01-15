#ifndef FACTORY_STATS_STAT_GRAPH_RENDER_H_
#define FACTORY_STATS_STAT_GRAPH_RENDER_H_

#include <Include/xrRender/StatGraphRender.h>


class fStatGraphRender
    : public IStatGraphRender
{
public:
    fStatGraphRender() = default;
    ~fStatGraphRender() = default;

    void Copy(IStatGraphRender& obj) override;
    void OnDeviceCreate() override;
    void OnDeviceDestroy() override;
    void OnRender(CStatGraph& owner) override;
};

#endif // FACTORY_STATS_STAT_GRAPH_RENDER_H_
