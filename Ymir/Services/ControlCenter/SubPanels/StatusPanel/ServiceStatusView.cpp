#include "ServiceStatusView.hpp"
#include "../../ControlCenterService.hpp"
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>

namespace Ymir
{

    ServiceStatusView::ServiceStatusView(ControlCenterService *host) : Host(host)
    {
        setLoadLaterWhenInvisible(true);

        Timer = addChild(std::make_unique<Wt::WTimer>());
        Timer->setInterval(std::chrono::milliseconds(800));
        Timer->timeout().connect([this](){
            Update(false);
        });
        Timer->start();

        auto* vertical_layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
        setLayoutSizeAware(true);

        Table = vertical_layout->addWidget(std::make_unique<Wt::WTable>());
        Table->elementAt(0, 0)->addNew<Wt::WText>("Name");
        Table->elementAt(0, 1)->addNew<Wt::WText>("Status");
        Table->elementAt(0, 2)->addNew<Wt::WText>("Operation");
        Table->setHeaderCount(1);
        Table->setStyleClass("table");
        Table->setHeight(Wt::WLength(80, Wt::LengthUnit::ViewportHeight));
        Update(true);
    }

    void ServiceStatusView::Update(bool initial)
    {
        if (!initial && !isVisible()) return;

        auto services = Host->GetApplication()->QueryServiceList();

        int current_service_count = Table->rowCount();
        int row_index = 1;
        for (auto service : services)
        {
            Wt::WText* service_status;
            Wt::WPushButton* service_operation;

            // Two rows for one service record.
            if (initial || row_index >= current_service_count)
            {
                Table->elementAt(row_index, 0)->addNew<Wt::WText>(service->Name);
                service_status = Table->elementAt(row_index, 1)->addWidget(
                        std::make_unique<Wt::WText>());
                service_status->setTextFormat(Wt::TextFormat::UnsafeXHTML);
                service_operation = Table->elementAt(row_index, 2)->addWidget(
                        std::make_unique<Wt::WPushButton>());
            }
            else
            {
                service_status = dynamic_cast<Wt::WText*>(Table->elementAt(row_index, 1)->
                        widget(0));
                service_operation = dynamic_cast<Wt::WPushButton*>(Table->elementAt(row_index, 2)->
                        widget(0));
            }

            if (service->IsBroken() && service_operation->text() != "Reset")
            {
                service_status->setText("<font color=\"#B22222\">Broken</font>");
                service_operation->setText("Reset");
                service_operation->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-blue");
                service_operation->clicked().connect([service](){
                    service->Reset();
                });
            }
            else if (service->IsEnable() && service_operation->text() != "Disable")
            {
                service_status->setText("<font color=\"#32CD32\">Enable</font>");
                service_operation->setText("Disable");
                service_operation->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-red");
                service_operation->clicked().connect([service](){
                    service->SetEnable(false);
                });
            }
            else if (!service->IsEnable() && service_operation->text() != "Enable")
            {
                service_status->setText("<font color=\"#8B8B7A\">Disable</font>");
                service_operation->setText("Enable");

                service_operation->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-green");
                service_operation->clicked().connect([service](){
                    service->SetEnable(true);
                });
            }

            ++row_index;
        }
    }
}