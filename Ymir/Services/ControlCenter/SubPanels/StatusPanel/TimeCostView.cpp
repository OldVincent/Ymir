#include "TimeCostView.hpp"
#include "../../ControlCenterService.hpp"
#include <Wt/WVBoxLayout.h>
#include <Wt/WStandardItem.h>
#include <Wt/WItemDelegate.h>

namespace Ymir
{
    TimeCostView::TimeCostView(ControlCenterService *host) : Host(host)
    {
        setLoadLaterWhenInvisible(true);

        Timer = addChild(std::make_unique<Wt::WTimer>());
        Timer->setInterval(std::chrono::milliseconds(100));
        Timer->timeout().connect([this](){
            Update();
        });
        Timer->start();

        Data = std::make_shared<Wt::WStandardItemModel>(0,2);
        Data->setHeaderData(0, "Sample Index");
        Data->setHeaderData(1, "Milliseconds");

        auto* vertical_layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
        setLayoutSizeAware(true);

        Chart = vertical_layout->addWidget(std::make_unique<Wt::Chart::WCartesianChart>());
        Chart->setModel(Data);
        Chart->setXSeriesColumn(0);
        Chart->setType(Wt::Chart::ChartType::Scatter);
        Chart->setPlotAreaPadding(120, Wt::Side::Right);
        Chart->setPlotAreaPadding(40, Wt::Side::Top | Wt::Side::Bottom);
        auto series = std::make_unique<Wt::Chart::WDataSeries>(1, Wt::Chart::SeriesType::Line);
        series->setShadow(Wt::WShadow(3, 3, Wt::WColor(0, 0, 0, 127), 3));

        Chart->addSeries(std::move(series));
        Chart->resize(Wt::WLength(60, Wt::LengthUnit::ViewportWidth),
                      Wt::WLength(60, Wt::LengthUnit::ViewportHeight));
        Chart->setMargin(10, Wt::Side::Top | Wt::Side::Bottom);
        Chart->setMargin(Wt::WLength::Auto, Wt::Side::Left | Wt::Side::Right);
        Chart->setTitle("Time Cost (ms)");
        Chart->setLegendEnabled(true);
        Update();
    }

    void TimeCostView::Update()
    {
        if (!isVisible()) return;

        auto delta_time = Host->GetApplication()->GetLastUpdateDeltaTime();
        int row_index = Data->rowCount();
        auto sample_record = std::make_unique<Wt::WStandardItem>(1,2);
        Data->appendRow(std::move(sample_record));
        Data->setData(row_index, 0, TotalSampleIndex);
        Data->setData(row_index, 1, delta_time);

        ++TotalSampleIndex;

        if (Data->rowCount() > 100)
        {
            Data->removeRows(0, Data->rowCount() - 100);
        }
    }
}