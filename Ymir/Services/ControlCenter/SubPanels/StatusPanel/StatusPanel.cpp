#include "StatusPanel.hpp"
#include "../../ControlCenterService.hpp"
#include <Wt/WMenu.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>

#include "FacilityStatusView.hpp"
#include "ServiceStatusView.hpp"
#include "ModelStatusView.hpp"
#include "OrderStatusView.hpp"
#include "TimeCostView.hpp"
#include "LogView.hpp"
#include "VisionView.hpp"

namespace Ymir
{
    /// Constructor which will build up the page.
    StatusPanel::StatusPanel(ControlCenterService* host)
    {
        setLoadLaterWhenInvisible(true);
        setLayoutSizeAware(true);

        auto* horizontal_layout = setLayout(std::make_unique<Wt::WHBoxLayout>());

        auto contents = std::make_unique<Wt::WStackedWidget>();
        contents->setPadding(Wt::WLength(15), Wt::Side::Top);
        contents->setPadding(Wt::WLength(15), Wt::Side::Left);
        contents->setPadding(Wt::WLength(15), Wt::Side::Bottom);
        contents->setPadding(Wt::WLength(15), Wt::Side::Right);
        contents->setLoadLaterWhenInvisible(true);
        auto* menu = horizontal_layout->addWidget(std::make_unique<Wt::WMenu>(contents.get()), 1);
        menu->setHeight(Wt::WLength(85, Wt::LengthUnit::ViewportHeight));
        menu->setMinimumSize(Wt::WLength(100, Wt::LengthUnit::Pixel),
                             Wt::WLength(85, Wt::LengthUnit::ViewportHeight));
        menu->setMaximumSize(Wt::WLength(100, Wt::LengthUnit::Pixel),
                             Wt::WLength(85, Wt::LengthUnit::ViewportHeight));
        menu->setStyleClass("nav nav-pills nav-stacked");
        menu->addItem("Facilities", std::make_unique<FacilityStatusView>(host));
        menu->addItem("Services", std::make_unique<ServiceStatusView>(host));
        menu->addItem("Models", std::make_unique<ModelStatusView>(host));
        menu->addItem("Orders", std::make_unique<OrderStatusView>(host));
        menu->addItem("Visions", std::make_unique<VisionView>(host));
        menu->addItem("Time Cost", std::make_unique<TimeCostView>(host));
        menu->addItem("Logs", std::make_unique<LogView>(host));
        horizontal_layout->addWidget(std::move(contents), 10);
    }
}