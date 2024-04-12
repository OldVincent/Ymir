#include "VisionViewerService.hpp"

#include <opencv4/opencv2/opencv.hpp>

namespace Ymir
{
    using namespace Eitr;

    bool VisionViewerService::OnInstall(const boost::json::object &configuration)
    {
        auto vision_name = configuration.at("Vision").as_string();
        Vision = GetApplication()->GetModel<VisionModel>(vision_name.c_str());

        cv::namedWindow(Name);

        return true;
    }

    void VisionViewerService::OnUninstall()
    {
        cv::destroyWindow(Name);
    }

    void VisionViewerService::OnUpdate()
    {
        if (Vision->Picture.rows > 0 && Vision->Picture.cols > 0)
        {
            cv::imshow(Name, Vision->Picture);
        }
        else
        {
            GetRuntime()->Log.Record(Name, "The picture to show is empty.", LogRecorder::Level::Warning);
        }
    }
}