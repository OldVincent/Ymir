#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <Wt/WContainerWidget.h>

namespace Ymir
{
    class ControlCenterService;

    class StatusPanel : public Wt::WContainerWidget
    {
    public:
        explicit StatusPanel(ControlCenterService* host);

    };
}