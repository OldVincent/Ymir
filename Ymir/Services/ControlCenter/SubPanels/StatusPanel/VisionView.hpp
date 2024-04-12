#pragma once

#include <Wt/WContainerWidget.h>
#include "../../../../Models/VisionModel.hpp"

namespace Ymir
{
    class ControlCenterService;

    class VisionView : public Wt::WContainerWidget
    {
    protected:
        ControlCenterService* Host;

        Wt::WTimer* Timer;
        Wt::WImage* Image;
        Wt::WComboBox* List;
        VisionModel* VisionToDisplay;

    public:
        explicit VisionView(ControlCenterService* host);

        void UpdateImage();
        void UpdateList();
        void ChangeVision(const std::string& model_name);
    };
}
