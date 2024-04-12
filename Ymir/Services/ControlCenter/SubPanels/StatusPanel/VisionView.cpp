#include "VisionView.hpp"
#include "../../ControlCenterService.hpp"

#include <Wt/WTimer.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WComboBox.h>
#include <Wt/WImage.h>
#include <Wt/WPushButton.h>
#include <Wt/WMemoryResource.h>

#include <opencv4/opencv2/opencv.hpp>

namespace Ymir
{
    VisionView::VisionView(ControlCenterService *host) : Host(host), VisionToDisplay(nullptr)
    {
        setLoadLaterWhenInvisible(true);

        Timer = addChild(std::make_unique<Wt::WTimer>());
        Timer->setInterval(std::chrono::milliseconds(100));
        Timer->timeout().connect([this](){
            UpdateImage();
        });

        setLayoutSizeAware(true);
        auto* vertical_layout = setLayout(std::make_unique<Wt::WVBoxLayout>());
        auto* select_container = vertical_layout->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto* horizontal_layout = select_container->setLayout(std::make_unique<Wt::WHBoxLayout>());
        List = horizontal_layout->addWidget(std::make_unique<Wt::WComboBox>(), 1);
        List->setMargin(3, Wt::Side::Right);
        List->changed().connect([this, vision_list = List](){
            ChangeVision(vision_list->currentText().toUTF8());
        });

        auto* refresh_button = horizontal_layout->addWidget(std::make_unique<Wt::WPushButton>("Refresh"), 0);
        refresh_button->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-light");
        refresh_button->clicked().connect([this](){
            UpdateList();
        });
        auto* view_button = horizontal_layout->addWidget(std::make_unique<Wt::WPushButton>("View"), 0);
        view_button->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-blue");
        view_button->clicked().connect([this, vision_list = List](){
            ChangeVision(vision_list->currentText().toUTF8());
        });

        Image = vertical_layout->addWidget(std::make_unique<Wt::WImage>());
        Image->setAlternateText("No Vision");
        Image->setHeight(Wt::WLength(80, Wt::LengthUnit::ViewportHeight));

        UpdateList();
        Timer->start();
    }

    void VisionView::UpdateList()
    {
        List->clear();
        auto model_list = Host->GetApplication()->QueryModelList();
        for (const auto& [name, model] : model_list)
        {
            List->addItem(name);
        }
    }

    void VisionView::UpdateImage()
    {
        if (VisionToDisplay && isVisible())
        {
            std::vector<unsigned char> buffer;
            cv::imencode(".png", VisionToDisplay->Picture, buffer);
            auto memory_resource = std::make_shared<Wt::WMemoryResource>();
            memory_resource->setMimeType("mime/png");
            memory_resource->setData(buffer.data(), static_cast<int>(buffer.size()));
            Image->setImageLink(Wt::WLink(memory_resource));
        }
    }

    void VisionView::ChangeVision(const std::string &model_name)
    {
        VisionToDisplay = Host->GetApplication()->GetModel<VisionModel>(model_name);
    }
}