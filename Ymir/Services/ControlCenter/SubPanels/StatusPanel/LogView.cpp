#include "LogView.hpp"

#include <Wt/WVBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>
#include <Wt/WStandardItem.h>
#include <boost/filesystem.hpp>
#include <fstream>

namespace Ymir
{

    LogView::LogView(ControlCenterService *host) : Host(host)
    {
        setLoadLaterWhenInvisible(true);

        auto* vertical_layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
        setLayoutSizeAware(true);
        auto* log_list = vertical_layout->addWidget(std::make_unique<Wt::WComboBox>());
        log_list->setMargin(10, Wt::Side::Right);

        boost::filesystem::directory_iterator finder(".");
        for (const auto& path : finder)
        {
            if (boost::filesystem::is_regular_file(path) && path.path().extension() == ".log")
            {
                log_list->addItem(path.path().string());
            }
        }
        log_list->changed().connect([this, log_list](){
            DisplayLog(log_list->currentText().toUTF8());
        });

        Data = std::make_shared<Wt::WStandardItemModel>(0,4);

        Table = vertical_layout->addWidget(std::make_unique<Wt::WTableView>());
        Table->setModel(Data);
        Table->setHeight(Wt::WLength(70, Wt::LengthUnit::ViewportHeight));
        Table->setColumnResizeEnabled(true);
        Table->setAlternatingRowColors(true);
        Table->setColumnAlignment(0, Wt::AlignmentFlag::Center);
        Table->setHeaderAlignment(0, Wt::AlignmentFlag::Center);
        Table->setColumnAlignment(1, Wt::AlignmentFlag::Center);
        Table->setHeaderAlignment(1, Wt::AlignmentFlag::Center);
        Table->setColumnAlignment(2, Wt::AlignmentFlag::Center);
        Table->setHeaderAlignment(2, Wt::AlignmentFlag::Center);
        Table->setRowHeight(Wt::WLength(30, Wt::LengthUnit::Pixel));
        Table->setHeaderHeight(Wt::WLength(30, Wt::LengthUnit::Pixel));
        Table->setColumnWidth(0, Wt::WLength(60, Wt::LengthUnit::Pixel));
        Table->setColumnWidth(1, Wt::WLength(60, Wt::LengthUnit::Pixel));
        Table->setColumnWidth(2, Wt::WLength(120, Wt::LengthUnit::Pixel));
        Table->setColumnWidth(3, Wt::WLength(400, Wt::LengthUnit::Pixel));
        DisplayLog(log_list->currentText().toUTF8());
    }

    void LogView::DisplayLog(const std::string &file_name)
    {
        Data->removeRows(0, Data->rowCount());
        Data->setHeaderData(0, "Time");
        Data->setHeaderData(1, "Type");
        Data->setHeaderData(2, "Entity");
        Data->setHeaderData(3, "Text");

        std::ifstream file(file_name);
        std::string log_text;
        int row_count = 0;
        while (std::getline(file, log_text))
        {
            auto date_part_end = log_text.find_first_of(' ');
            auto date_part = log_text.substr(0, date_part_end);
            auto severity_part_begin = log_text.find_first_of('[') + 1;
            auto severity_part_end = log_text.find_first_of(']', severity_part_begin);
            auto severity_part = log_text.substr(severity_part_begin, severity_part_end - severity_part_begin);
            auto entity_part_begin = log_text.find_first_of(' ', severity_part_end) + 1;
            auto entity_part_end = log_text.find_first_of(':', entity_part_begin);
            std::string entity_part;
            std::string text_part;
            if (entity_part_end != std::string::npos)
            {
                entity_part = log_text.substr(entity_part_begin, entity_part_end - entity_part_begin);
                text_part = log_text.substr(entity_part_end + 1);
            }
            else
            {
                text_part = log_text.substr(entity_part_begin);
            }
            auto sample_record = std::make_unique<Wt::WStandardItem>(1,4);
            Data->appendRow(std::move(sample_record));
            Data->setData(row_count, 0, date_part);
            Data->setData(row_count, 1, severity_part);
            Data->setData(row_count, 2, entity_part);
            Data->setData(row_count, 3, text_part);
            ++row_count;
        }
    }
}