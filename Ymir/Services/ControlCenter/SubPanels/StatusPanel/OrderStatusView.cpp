#include "OrderStatusView.hpp"
#include "../../ControlCenterService.hpp"
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>

namespace Ymir
{
    OrderStatusView::OrderStatusView(ControlCenterService *host) : Host(host)
    {
        setLoadLaterWhenInvisible(true);

        Timer = addChild(std::make_unique<Wt::WTimer>());
        Timer->setInterval(std::chrono::milliseconds(1000));
        Timer->timeout().connect([this](){
            Update(false);
        });
        Timer->start();

        auto* vertical_layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
        setLayoutSizeAware(true);
        Table = vertical_layout->addWidget(std::make_unique<Wt::WTable>());

        Update(true);
    }

    void OrderStatusView::Update(bool initial)
    {
        if (!initial && !isVisible()) return;

        auto orders = Host->GetApplication()->QueryOrderList();
        Table->clear();
        Table->elementAt(0, 0)->addNew<Wt::WText>("Name");
        Table->elementAt(0, 1)->addNew<Wt::WText>("Status");
        Table->elementAt(0, 2)->addNew<Wt::WText>("Operation");
        Table->setHeaderCount(1);
        Table->setStyleClass("table");
        Table->setHeight(Wt::WLength(75, Wt::LengthUnit::ViewportHeight));
        int row_index = 1;
        for (auto [name, order] : orders)
        {
            Wt::WText* service_status;
            Wt::WPushButton* service_operation;

            Table->elementAt(row_index, 0)->addNew<Wt::WText>(name);
            if (order->IsEmpty())
            {
                Table->elementAt(row_index, 1)->addNew<Wt::WText>("<font color=\"#8B8B7A\">Empty</font>")->
                        setTextFormat(Wt::TextFormat::UnsafeXHTML);
            }
            else
            {
                Table->elementAt(row_index, 1)->addNew<Wt::WText>("<font color=\"#32CD32\">Laden</font>")->
                        setTextFormat(Wt::TextFormat::UnsafeXHTML);
            }

            auto* button = Table->elementAt(row_index, 2)->addWidget(
                            std::make_unique<Wt::WPushButton>("Clear"));
            button->clicked().connect([order = order](){
                order->Clear();
            });
            button->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-blue");

            ++row_index;
        }
    }
}