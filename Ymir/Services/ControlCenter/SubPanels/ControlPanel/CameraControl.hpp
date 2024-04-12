#pragma once

#include <Wt/WContainerWidget.h>
#include "../../../../Models/VisionModel.hpp"
#include "../../../../Orders/CameraSettingOrder.hpp"

namespace Ymir
{
    class ControlCenterService;

    class CameraControl : public Wt::WContainerWidget
    {
    protected:
        ControlCenterService* Host;

        Wt::WTimer* Timer;
        Wt::WImage* VisionImage;
        Wt::WComboBox* VisionList;
        Wt::WComboBox* OrderList;
        Wt::WSlider* ExposureSlider;
        Wt::WSlider* GainSlider;
        Wt::WSlider* WhiteBalanceRedSlider;
        Wt::WSlider* WhiteBalanceBlueSlider;

        Wt::WText* ExposureLabel;
        Wt::WText* GainLabel;
        Wt::WText* WhiteBalanceRedLabel;
        Wt::WText* WhiteBalanceBlueLabel;

        VisionModel* Vision {nullptr};
        CameraSettingOrder* SettingOrder {nullptr};

    public:
        explicit CameraControl(ControlCenterService* host);

        void UpdateImage();
        void UpdateList();
        void UpdateSettings();
        void ChangeVision(const std::string& model_name);
        void ChangeOrder(const std::string& order_name);
    };
}
