#include "MainPanel.hpp"
#include "ControlCenterService.hpp"

#include <Wt/WApplication.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WEnvironment.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WMenu.h>
#include <Wt/WMessageBox.h>
#include <Wt/WNavigationBar.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPopupMenuItem.h>
#include <Wt/WText.h>
#include <Wt/WPainter.h>
#include <Wt/WPaintDevice.h>
#include <Wt/WImage.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>

#include "SubPanels/StatusPanel/StatusPanel.hpp"
#include "SubPanels/ControlPanel/ControlPanel.hpp"

namespace Ymir
{
    using namespace Wt;

    MainPanel::MainPanel(const Wt::WEnvironment &environment, ControlCenterService *host) :
            WApplication(environment), Host(host)
    {
        Host->WebPanel = this;

        //============================================================
        // Global settings.
        //============================================================
        setTitle("Ymir");

        //============================================================
        // Theme settings.
        //============================================================
        auto bootstrapTheme = std::make_shared<WBootstrapTheme>();
        bootstrapTheme->setVersion(BootstrapVersion::v3);
        bootstrapTheme->setResponsive(true);
        setTheme(bootstrapTheme);

        useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");
        useStyleSheet("Style/PrettyButtons.css");

        //============================================================
        // Build page body.
        //============================================================

        auto* main_body = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
        main_body->resize(WLength(100, LengthUnit::ViewportWidth),
                          WLength(100, LengthUnit::ViewportHeight));
        auto* main_layout = main_body->setLayout(std::make_unique<WVBoxLayout>());
        main_layout->setContentsMargins(0, 0, 0, 0);

        //============================================================
        // Build navigation bar.
        //============================================================

        // Navigation bar.
        auto* navigation = main_layout->addWidget(std::make_unique<Wt::WNavigationBar>(), 0);
        navigation->setTitle("Ymir Control Center");
        navigation->setResponsive(true);
        navigation->addStyleClass("navbar-collapse");

        auto* contents_stack = main_layout->addWidget(std::make_unique<Wt::WStackedWidget>(), 1);
        contents_stack->addStyleClass("contents");
        contents_stack->setLoadLaterWhenInvisible(true);

        //------------------------------------------------------------
        // Build categories menu.
        //------------------------------------------------------------
        auto* menu_categories = navigation->addMenu(std::make_unique<Wt::WMenu>(contents_stack));
        menu_categories->setStyleClass("navbar-left navbar-nav nav");
        menu_categories->addItem("Status", std::make_unique<StatusPanel>(Host));
        menu_categories->addItem("Control", std::make_unique<ControlPanel>(Host));
        //------------------------------------------------------------
        // Build information menu.
        //------------------------------------------------------------
        auto* menu_information = navigation->addMenu(std::make_unique<Wt::WMenu>(),
                                                    Wt::AlignmentFlag::Right);
        auto* item_about = menu_information->addItem("About");
        auto popup_menu_about = std::make_unique<Wt::WPopupMenu>();
        popup_menu_about->addItem("Author");
        popup_menu_about->addItem("Version");
        popup_menu_about->itemSelected().connect([host_menu = popup_menu_about.get()] (Wt::WMenuItem *item) {
            if (item->text() == "Author")
            {
                auto messageBox = host_menu->addChild(
                        std::make_unique<Wt::WMessageBox>
                                ("About",
                                 Wt::WString("<p>Author: <br/>"
                                             "RoboPioneers Vision Group, Gaia Committee. "
                                             "<hr/>"
                                             "Ymir is an auto attack system developed and maintained"
                                             " by RoboPioneers Vision Group. "
                                             "All rights reserved until being open-source.</p>"),
                                 Wt::Icon::Information,
                                 Wt::StandardButton::Ok));
                messageBox->buttonClicked().connect([host_menu, messageBox] {
                    host_menu->removeChild(messageBox);
                });
                messageBox->show();
                return;
            }
            if (item->text() == "Version")
            {
                auto messageBox = host_menu->addChild(
                        std::make_unique<Wt::WMessageBox>
                                ("Version",
                                 Wt::WString("<p>Ymir Alpha0.8.0 <br/>"
                                             "Ymir Control Center Alpha0.8.0"
                                             "</p>"),
                                 Wt::Icon::Information,
                                 Wt::StandardButton::Ok));
                messageBox->buttonClicked().connect([host_menu, messageBox] {
                    host_menu->removeChild(messageBox);
                });
                messageBox->show();
                return;
            }
        });
        item_about->setMenu(std::move(popup_menu_about));
    }
}