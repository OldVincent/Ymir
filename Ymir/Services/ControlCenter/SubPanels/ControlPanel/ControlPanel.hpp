#pragma once

#include <Wt/WContainerWidget.h>

namespace Ymir
{
    class ControlCenterService;

    class ControlPanel : public Wt::WContainerWidget
    {
    protected:
        ControlCenterService* Host;

    public:
        explicit ControlPanel(ControlCenterService* host);
    };
}