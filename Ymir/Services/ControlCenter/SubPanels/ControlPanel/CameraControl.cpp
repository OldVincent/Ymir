#include "CameraControl.hpp"
#include "../../ControlCenterService.hpp"
#include <Wt/WTimer.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WComboBox.h>
#include <Wt/WImage.h>
#include <Wt/WPushButton.h>
#include <Wt/WMemoryResource.h>
#include <Wt/WSlider.h>

#include <opencv4/opencv2/opencv.hpp>

namespace Ymir
{

    CameraControl::CameraControl(ControlCenterService *host) : Host(host)
    {
        setLoadLaterWhenInvisible(true);

        Timer = addChild(std::make_unique<Wt::WTimer>());
        Timer->setInterval(std::chrono::milliseconds(100));
        Timer->timeout().connect([this](){
            UpdateImage();
            UpdateSettings();
        });

        setLayoutSizeAware(true);
        auto* vertical_layout = setLayout(std::make_unique<Wt::WVBoxLayout>());

        auto* selector_container = vertical_layout->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto* selector_layout = selector_container->setLayout(std::make_unique<Wt::WHBoxLayout>());
        VisionList = selector_layout->addWidget(std::make_unique<Wt::WComboBox>(), 1);
        VisionList->setMargin(3, Wt::Side::Right);
        VisionList->changed().connect([this, vision_list = VisionList](){
            ChangeVision(vision_list->currentText().toUTF8());
        });
        OrderList = selector_layout->addWidget(std::make_unique<Wt::WComboBox>(), 1);
        OrderList->setMargin(3, Wt::Side::Right);
        OrderList->changed().connect([this, order_list = OrderList](){
            ChangeOrder(order_list->currentText().toUTF8());
        });
        auto* refresh_button = selector_layout->addWidget(std::make_unique<Wt::WPushButton>("Refresh"), 0);
        refresh_button->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-light");
        refresh_button->clicked().connect([this](){
            UpdateList();
        });
        auto* choose_button = selector_layout->addWidget(std::make_unique<Wt::WPushButton>("Choose"), 0);
        choose_button->setStyleClass("pretty-btn pretty-btn-sm pretty-btn-blue");
        choose_button->clicked().connect([this, vision_list = VisionList, order_list = OrderList](){
            ChangeVision(vision_list->currentText().toUTF8());
            ChangeOrder(order_list->currentText().toUTF8());
        });

        VisionImage = vertical_layout->addWidget(std::make_unique<Wt::WImage>());
        VisionImage->setAlternateText("No Vision");
        VisionImage->setMinimumSize(Wt::WLength(640, Wt::LengthUnit::Pixel),
                                    Wt::WLength(360, Wt::LengthUnit::Pixel));
        VisionImage->setMaximumSize(Wt::WLength(640, Wt::LengthUnit::Pixel),
                                    Wt::WLength(360, Wt::LengthUnit::Pixel));
        VisionImage->setMargin(Wt::WLength::Auto, Wt::Side::Left | Wt::Side::Right);

        auto* exposure_group = vertical_layout->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto* exposure_layout = exposure_group->setLayout(std::make_unique<Wt::WHBoxLayout>());
        exposure_layout->addWidget(std::make_unique<Wt::WText>("Exposure"));
        ExposureLabel = exposure_layout->addWidget(std::make_unique<Wt::WText>("0"));
        ExposureSlider = exposure_layout->addWidget(std::make_unique<Wt::WSlider>(), 2);
        ExposureSlider->setValueText("Exposure");
        ExposureSlider->setTickPosition(Wt::WSlider::TickPosition::TicksAbove);
        ExposureSlider->setTickInterval(1000);
        ExposureSlider->setMinimum(0);
        ExposureSlider->setMaximum(30000);

        auto* gain_group = vertical_layout->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto* gain_layout = gain_group->setLayout(std::make_unique<Wt::WHBoxLayout>());
        gain_layout->addWidget(std::make_unique<Wt::WText>("Gain    "));
        GainLabel = gain_layout->addWidget(std::make_unique<Wt::WText>("0"));
        GainSlider = gain_layout->addWidget(std::make_unique<Wt::WSlider>());
        GainSlider->setValueText("Gain");
        GainSlider->setTickPosition(Wt::WSlider::TickPosition::TicksAbove);
        GainSlider->setTickInterval(1);
        GainSlider->setMinimum(0);
        GainSlider->setMaximum(16);

        auto* balance_red_group = vertical_layout->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto* balance_red_layout = balance_red_group->setLayout(std::make_unique<Wt::WHBoxLayout>());
        balance_red_layout->addWidget(std::make_unique<Wt::WText>("White Balance Red "));
        WhiteBalanceRedLabel = balance_red_layout->addWidget(std::make_unique<Wt::WText>("0"));
        WhiteBalanceRedSlider = balance_red_layout->addWidget(std::make_unique<Wt::WSlider>());
        WhiteBalanceRedSlider->setValueText("White Balance: Red");
        WhiteBalanceRedSlider->setTickPosition(Wt::WSlider::TickPosition::TicksAbove);
        WhiteBalanceRedSlider->setTickInterval(10000);
        WhiteBalanceRedSlider->setMinimum(0);
        WhiteBalanceRedSlider->setMaximum(20000);

        auto* balance_blue_group = vertical_layout->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto* balance_blue_layout = balance_blue_group->setLayout(std::make_unique<Wt::WHBoxLayout>());
        balance_blue_layout->addWidget(std::make_unique<Wt::WText>("White Balance Blue"));
        WhiteBalanceBlueLabel = balance_blue_layout->addWidget(std::make_unique<Wt::WText>("0"));
        WhiteBalanceBlueSlider = balance_blue_layout->addWidget(std::make_unique<Wt::WSlider>());
        WhiteBalanceBlueSlider->setValueText("White Balance: Blue");
        WhiteBalanceBlueSlider->setTickInterval(10000);
        WhiteBalanceBlueSlider->setMinimum(0);
        WhiteBalanceBlueSlider->setMaximum(20000);

        UpdateList();
        Timer->start();
    }

    void CameraControl::UpdateImage()
    {
        if (Vision && isVisible())
        {
            std::vector<unsigned char> buffer;
            cv::Mat resized_picture;
            cv::resize(Vision->Picture, resized_picture, cv::Size(640, 360));
            cv::imencode(".png", Vision->Picture, buffer);
            auto memory_resource = std::make_shared<Wt::WMemoryResource>();
            memory_resource->setMimeType("mime/png");
            memory_resource->setData(buffer.data(), static_cast<int>(buffer.size()));
            VisionImage->setImageLink(Wt::WLink(memory_resource));
        }
    }

    void CameraControl::UpdateList()
    {
        if (!isVisible()) return;
        VisionList->clear();
        auto model_list = Host->GetApplication()->QueryModelList();
        for (const auto& [name, model] : model_list)
        {
            if (dynamic_cast<VisionModel*>(model))
            {
                VisionList->addItem(name);
            }
        }
        OrderList->clear();
        auto order_list = Host->GetApplication()->QueryOrderList();
        for (const auto& [name, model] : order_list)
        {
            if (dynamic_cast<CameraSettingOrder*>(model))
            {
                OrderList->addItem(name);
            }
        }
    }

    void CameraControl::UpdateSettings()
    {
        if (SettingOrder && isVisible())
        {
            SettingOrder->Exposure = ExposureSlider->value();
            SettingOrder->Gain = GainSlider->value();
            SettingOrder->WhiteBalanceRed = WhiteBalanceRedSlider->value();
            SettingOrder->WhiteBalanceBlue = WhiteBalanceBlueSlider->value();

            ExposureLabel->setText(std::to_string(ExposureSlider->value()));
            GainLabel->setText(std::to_string(GainSlider->value()));
            WhiteBalanceRedLabel->setText(std::to_string(WhiteBalanceRedSlider->value()));
            WhiteBalanceBlueLabel->setText(std::to_string(WhiteBalanceBlueSlider->value()));
        }
    }

    void CameraControl::ChangeVision(const std::string &model_name)
    {
        if (!model_name.empty())
        {
            Vision = Host->GetApplication()->GetModel<VisionModel>(model_name);
        }
    }

    void CameraControl::ChangeOrder(const std::string& order_name)
    {
        if (!order_name.empty())
        {
            SettingOrder = dynamic_cast<CameraSettingOrder*>(Host->GetApplication()->GetOrder<Eitr::Order>(order_name));
            if (SettingOrder->LastExposure >= 0)
                ExposureSlider->setValue(SettingOrder->LastExposure);
            if (SettingOrder->LastGain >= 0)
                GainSlider->setValue(SettingOrder->LastGain);
            if (SettingOrder->WhiteBalanceRed >= 0)
                WhiteBalanceRedSlider->setValue(SettingOrder->WhiteBalanceRed);
            if (SettingOrder->LastWhiteBalanceBlue >= 0)
                WhiteBalanceBlueSlider->setValue(SettingOrder->LastWhiteBalanceBlue);
        }
    }
}