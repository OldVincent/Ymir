#include "ControlPanel.hpp"

#include <Wt/WHBoxLayout.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>
#include "ConfigurationView.hpp"
#include "CameraControl.hpp"

namespace Ymir
{
    ControlPanel::ControlPanel(ControlCenterService *host) : Host(host)
    {
        setLayoutSizeAware(true);
        setLoadLaterWhenInvisible(true);

        auto* horizontal_layout = setLayout(std::make_unique<Wt::WHBoxLayout>());

        auto contents = std::make_unique<Wt::WStackedWidget>();
        contents->setPadding(Wt::WLength(15), Wt::Side::Top);
        contents->setPadding(Wt::WLength(15), Wt::Side::Left);
        contents->setPadding(Wt::WLength(15), Wt::Side::Bottom);
        contents->setPadding(Wt::WLength(15), Wt::Side::Right);
        contents->setLoadLaterWhenInvisible(true);
        auto* menu = horizontal_layout->addWidget(std::make_unique<Wt::WMenu>(contents.get()), 1);
        menu->setHeight(Wt::WLength(85, Wt::LengthUnit::ViewportHeight));
        menu->setMinimumSize(Wt::WLength(120, Wt::LengthUnit::Pixel),
                             Wt::WLength(85, Wt::LengthUnit::ViewportHeight));
        menu->setMaximumSize(Wt::WLength(120, Wt::LengthUnit::Pixel),
                             Wt::WLength(85, Wt::LengthUnit::ViewportHeight));
        menu->setStyleClass("nav nav-pills nav-stacked");
        menu->addItem("Configurations", std::make_unique<ConfigurationView>(host));
        menu->addItem("Cameras", std::make_unique<CameraControl>(host));

        horizontal_layout->addWidget(std::move(contents), 10);
    }
}