#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTimer.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/Chart/WCartesianChart.h>
#include <memory>

namespace Ymir
{
    class ControlCenterService;

    class TimeCostView : public Wt::WContainerWidget
    {
    protected:
        ControlCenterService* Host;

        Wt::WTimer* Timer;
        std::shared_ptr<Wt::WStandardItemModel> Data;
        Wt::Chart::WCartesianChart* Chart;
        int TotalSampleIndex = 1;

    public:
        explicit TimeCostView(ControlCenterService* host);

        /**
        * @brief Update the service table view.
        * @param initial Whether the service table is created or not.
        */
        void Update();
    };
}
