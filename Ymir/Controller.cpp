#include "Controller.hpp"

#include <opencv4/opencv2/opencv.hpp>

namespace Ymir
{
    bool Controller::OnInstall()
    {
        LoadConfiguration("./YmirSettings.json");
        return true;
    }

    void Controller::OnUninstall()
    {
    }

    bool Controller::OnUpdate()
    {
        if (cv::waitKey(1) == 27)
        {
            return false;
        }
        return true;
    }
}