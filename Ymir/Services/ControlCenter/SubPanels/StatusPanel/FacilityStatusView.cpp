#include "FacilityStatusView.hpp"
#include "../../ControlCenterService.hpp"
#include <Wt/WText.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WTimer.h>
#include <Wt/WPushButton.h>

namespace Ymir
{

    FacilityStatusView::FacilityStatusView(ControlCenterService *host) : Host(host)
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

    void FacilityStatusView::Update(bool initial)
    {
        if (!initial && !isVisible()) return;

        auto facilities = Host->GetApplication()->QueryFacilityList();

        int current_service_count = Table->rowCount();
        int row_index = 1;
        for (auto facility : facilities)
        {
            Wt::WText* facility_status;

            // Two rows for one service record.
            if (initial || row_index >= current_service_count)
            {
                Table->elementAt(row_index, 0)->addNew<Wt::WText>(facility->Name);
                facility_status = Table->elementAt(row_index, 1)->addWidget(
                        std::make_unique<Wt::WText>());
                facility_status->setTextFormat(Wt::TextFormat::UnsafeXHTML);
                auto* facility_reset = Table->elementAt(row_index, 2)->addWidget(
                        std::make_unique<Wt::WPushButton>("Reset"));
                facility_reset->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-blue");
                facility_reset->clicked().connect([facility, facility_status](){
                    facility->Reset();
                });
            }
            else
            {
                facility_status = dynamic_cast<Wt::WText*>(Table->elementAt(row_index, 1)->
                        widget(0));
            }

            if (facility->IsBroken())
            {
                facility_status->setText("<font color=\"#B22222\">Broken</font>");

            }
            else if (facility->IsEnable())
            {
                facility_status->setText("<font color=\"#32CD32\">Enable</font>");
            }
            else if (!facility->IsEnable())
            {
                facility_status->setText("<font color=\"#8B8B7A\">Disable</font>");
            }

            ++row_index;
        }
    }
}