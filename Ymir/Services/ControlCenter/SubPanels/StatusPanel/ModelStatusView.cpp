#include "ModelStatusView.hpp"
#include "../../ControlCenterService.hpp"
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>

namespace Ymir
{
    ModelStatusView::ModelStatusView(ControlCenterService* host) : Host(host)
    {
        setLoadLaterWhenInvisible(true);

        Timer = addChild(std::make_unique<Wt::WTimer>());
        Timer->setInterval(std::chrono::milliseconds(3000));
        Timer->timeout().connect([this](){
            Update(false);
        });
        Timer->start();

        auto* vertical_layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
        setLayoutSizeAware(true);

        Table = vertical_layout->addWidget(std::make_unique<Wt::WTable>());
        Update(true);
    }

    void ModelStatusView::Update(bool initial)
    {
        if (!initial && !isVisible()) return;

        auto models = Host->GetApplication()->QueryModelList();
        Table->clear();
        Table->elementAt(0, 0)->addNew<Wt::WText>("Name");
        Table->elementAt(0, 1)->addNew<Wt::WText>("Operation");
        Table->setHeaderCount(1);
        Table->setStyleClass("table");
        Table->setHeight(Wt::WLength(75, Wt::LengthUnit::ViewportHeight));
        int row_index = 1;
        for (auto [name, model] : models)
        {
            Table->elementAt(row_index, 0)->addNew<Wt::WText>(name);

            auto* button = Table->elementAt(row_index, 1)->addWidget(
                    std::make_unique<Wt::WPushButton>("Reset"));
            button->clicked().connect([model = model](){
                model->Reset();
            });
            button->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-blue");

            ++row_index;
        }
    }
}