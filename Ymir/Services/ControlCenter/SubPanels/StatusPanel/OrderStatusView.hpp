#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <Wt/WContainerWidget.h>
#include <Wt/WTimer.h>
#include <Wt/WTable.h>

namespace Ymir
{
    class ControlCenterService;

    class OrderStatusView : public Wt::WContainerWidget
    {
    protected:
        ControlCenterService* Host;

        Wt::WTimer* Timer;
        Wt::WTable* Table;

    public:
        explicit OrderStatusView(ControlCenterService* host);

        /**
         * @brief Update the service table view.
         * @param initial Whether the service table is created or not.
         */
        void Update(bool initial = false);
    };
}