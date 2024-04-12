#include "DahengCameraReaderService.hpp"

#include <GaiaExceptions/GaiaExceptions.hpp>
#include "../Models/VisionModel.hpp"

#include <cmath>

namespace Ymir
{
    using namespace Eitr;

    bool DahengCameraReaderService::OnInstall(const boost::json::object &configuration)
    {
        std::string camera_name {configuration.at("Camera").as_string().c_str()};

        Camera = GetApplication()->GetFacility<DahengCameraFacility>(camera_name);

        Gaia::Exceptions::NullPointerException::ThrowIfNull(Camera, "CameraFacility");

        GetRuntime()->Log.Record(Name,
                                 "Camera facility " + camera_name + " connected.",
                                 LogRecorder::Level::Milestone);

        LastPictureTimestamp = std::chrono::steady_clock::now();

        std::string output_vision_name = Name;

        ApplyOption(configuration, "Output", [&output_vision_name](const boost::json::value& value){
            output_vision_name = std::string(value.as_string().c_str());
            return true;
        }, true);

        Vision = GetApplication()->GetModel<VisionModel>(output_vision_name);

        ApplyOption(configuration, "ResizeMethod", [this](const boost::json::value& value){
            if (value.as_string() == "Resize")
            {
                ResizeMethod = ResizeMethodType::Resize;
                return true;
            }
            if (value.as_string() == "Crop")
            {
                ResizeMethod = ResizeMethodType::Crop;
                return true;
            }
            return false;
        }, true);

        ApplyOption(configuration, "ResizeWidth", [this](const boost::json::value& value){
            TargetSize.width = static_cast<int>(value.as_int64());
            return true;
        }, true);

        ApplyOption(configuration, "ResizeHeight", [this](const boost::json::value& value){
            TargetSize.height = static_cast<int>(value.as_int64());
            return true;
        }, true);

        ApplyOption(configuration, "PictureOvertime", [this](const boost::json::value& value){
            TimestampOvertimeMilliseconds = value.as_int64();
            return true;
        }, true);

        return true;
    }

    void DahengCameraReaderService::OnUninstall()
    {
        GetRuntime()->Log.Record(Name,
                                 "Camera index closed.",
                                 LogRecorder::Level::Message);
    }

    void DahengCameraReaderService::OnUpdate()
    {
        //============================================================
        // Get raw picture data from the camera.
        //============================================================

        auto picture = Camera->GetCamera()->RetrievePicture();

        if (picture.GetHeight() == 0 || picture.GetWidth() == 0)
        {
            GetRuntime()->Log.Record(Name, "Waiting for camera's initialization.",
                                     LogRecorder::Level::Warning);
            std::this_thread::sleep_for(Camera->GetCamera()->GetExposureTime() * 1.5f);

            if (picture.GetHeight() == 0 || picture.GetWidth() == 0)
            {
                GetRuntime()->Log.Record(Name, "Camera can not initialize. Service disabled.",
                                         LogRecorder::Level::Error);
                SetEnable(false);
                return;
            }
        }

        if (TimestampOvertimeMilliseconds > 0 && std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::steady_clock::now() - picture.GetTimestamp()).count() > TimestampOvertimeMilliseconds)
        {
            GetRuntime()->Log.Record(Name,
                                     "Picture overtime. Camera is considered to be offline. Service disabled.",
                                     LogRecorder::Level::Error);
            SetEnable(false);
        }

        //============================================================
        // Convert the raw picture data into a BGR picture.
        //============================================================

        auto picture_bayer_bg = cv::Mat(
                cv::Size(static_cast<int>(picture.GetWidth()),
                         static_cast<int>(picture.GetHeight())),
                CV_8UC1, picture.GetData());
        cv::Mat picture_bgr;
        cv::cvtColor(picture_bayer_bg, picture_bgr, cv::COLOR_BayerBG2BGR);

        //============================================================
        // Resize the picture.
        //============================================================

        if (ResizeMethod != ResizeMethodType::None)
        {
            if (TargetSize.width <= 0 || TargetSize.width > picture_bgr.cols)
            {
                TargetSize.width = picture_bgr.cols;
            }
            if (TargetSize.height <= 0 || TargetSize.width > picture_bgr.cols)
            {
                TargetSize.height = picture_bgr.rows;
            }

            if (ResizeMethod == ResizeMethodType::Resize)
            {
                cv::resize(picture_bgr, picture_bgr, TargetSize);
            }
            else
            {
                int target_area_x = (picture_bgr.cols - TargetSize.width) / 2;
                int target_area_y = (picture_bgr.rows - TargetSize.height) / 2;
                cv::Rect target_area(target_area_x, target_area_y, TargetSize.width, TargetSize.height);
                picture_bgr = picture_bgr(target_area);
            }
        }

        //============================================================
        // Assign the picture to the target model.
        //============================================================

        Vision->Picture = picture_bgr;
        Vision->Timestamp = picture.GetTimestamp();
    }
}