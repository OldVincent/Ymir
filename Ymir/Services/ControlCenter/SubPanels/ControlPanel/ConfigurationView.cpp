#include "ConfigurationView.hpp"

#include "../../ControlCenterService.hpp"
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPushButton.h>
#include <boost/filesystem.hpp>

#include <fstream>
#include <sstream>

namespace Ymir
{

    ConfigurationView::ConfigurationView(ControlCenterService *host) : Host(host)
    {
        setLoadLaterWhenInvisible(true);
        setLayoutSizeAware(true);

        auto* vertical_layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
        auto* select_container = vertical_layout->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto* horizontal_layout = select_container->setLayout(std::make_unique<Wt::WHBoxLayout>());
        auto* log_list = horizontal_layout->addWidget(std::make_unique<Wt::WComboBox>(), 1);
        log_list->setMargin(3, Wt::Side::Right);
        auto* save_button = horizontal_layout->addWidget(std::make_unique<Wt::WPushButton>("Save"), 0);
        save_button->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-blue");
        save_button->clicked().connect([this](){
            SaveConfiguration();
        });

        TextEditor = vertical_layout->addWidget(std::make_unique<Wt::WTextArea>());
        TextEditor->setHeight(Wt::WLength(80, Wt::LengthUnit::ViewportHeight));

        boost::filesystem::recursive_directory_iterator finder(".");
        for (const auto& path : finder)
        {
            if (boost::filesystem::is_regular_file(path) && path.path().extension() == ".json")
            {
                log_list->addItem(path.path().string());
            }
        }
        log_list->changed().connect([this, log_list](){
            OpenConfiguration(log_list->currentText().toUTF8());
        });
        OpenConfiguration(log_list->currentText().toUTF8());
    }

    void ConfigurationView::OpenConfiguration(const std::string &path)
    {
        OpenedConfigurationPath = path;
        std::ifstream file(path);
        std::string text_line;
        std::stringstream text_builder;
        while (std::getline(file, text_line))
        {
            text_builder << text_line + "\n";
        }
        TextEditor->setText(text_builder.str());
    }

    void ConfigurationView::SaveConfiguration()
    {
        std::ofstream file(OpenedConfigurationPath, std::ios::out);
        file << TextEditor->text().toUTF8();
        file.flush();
        file.close();
    }
}