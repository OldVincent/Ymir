#pragma once
#include <Wt/WApplication.h>

namespace Ymir
{
    class ControlCenterService;

    class MainPanel : public Wt::WApplication
    {
    private:
        ControlCenterService* Host;

    public:
        MainPanel(const Wt::WEnvironment& environment, ControlCenterService* host);
    };
}