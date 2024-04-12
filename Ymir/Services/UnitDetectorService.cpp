#include "UnitDetectorService.hpp"

#include <boost/filesystem.hpp>
#include <opencv4/opencv2/tracking.hpp>

namespace Ymir
{
    /// Install and load the network.
    bool UnitDetectorService::OnInstall(const boost::json::object &configuration)
    {
        std::string model_path {configuration.at("Model").as_string().c_str()};
        std::string weight_path = model_path + ".weights";
        std::string cfg_path = model_path + ".cfg";

        auto network_width = static_cast<int>(configuration.at("Width").as_int64());
        auto network_height = static_cast<int>(configuration.at("Height").as_int64());

        Detector = std::make_unique<Gaia::DNNHelper::YoloV4>();
        Detector->Initialize(cfg_path, weight_path, cv::Size(network_width, network_height));

        auto threshold_finder = configuration.find("Threshold");
        if (threshold_finder != configuration.end())
        {
            ConfidenceThreshold = threshold_finder->value().as_double();
        }

        ApplyOption(configuration, "SourceVision", [this](const boost::json::value& value){
            SourceVision = GetApplication()->GetModel<VisionModel>(value.as_string().c_str());
            return true;
        });

        auto result_finder = configuration.find("ResultVision");
        if (result_finder != configuration.end())
        {
            EnableResultHighlighting = true;
            std::string result_vision {result_finder->value().as_string().c_str()};
            ResultVision = GetApplication()->GetModel<VisionModel>(result_vision);
        }

        std::string result_target = Name;
        auto result_target_finder = configuration.find("ResultTarget");
        if (result_target_finder != configuration.end())
        {
            result_target = std::string(result_target_finder->value().as_string().c_str());
        }
        ResultTarget = GetApplication()->GetModel<TargetModel>(result_target);

        return true;
    }
    /// Uninstall.
    void UnitDetectorService::OnUninstall()
    {
        Detector.reset();
    }

    void UnitDetectorService::OnUpdate()
    {
        if (SourceVision->Picture.empty()) return;

        if (EnableResultHighlighting)
        {
            SourceVision->Picture.copyTo(ResultVision->Picture);
        }

        ResultTarget->Picture = SourceVision->Picture;

        //============================================================
        // Try to track the previous target.
        //============================================================
        if (!ResultTarget->Tracker.empty() && ResultTarget->TrackerTimestamp.IsValid())
        {
            if (ResultTarget->Tracker->update(SourceVision->Picture, ResultTarget->BoundingBox))
            {
                if (EnableResultHighlighting)
                {
                    cv::rectangle(ResultVision->Picture, ResultTarget->BoundingBox,
                                  cv::Scalar(152, 255, 152),3);
                }
                auto& box = ResultTarget->BoundingBox;
                if (box.x > 0 && box.y > 0 &&
                    box.x < SourceVision->Picture.cols && box.y < SourceVision->Picture.rows &&
                    box.x + box.width <= SourceVision->Picture.cols &&
                    box.y + box.height <= SourceVision->Picture.rows && box.width * box.height > 0)
                {
                    ResultTarget->FoundUnit = true;
                    return;
                }
            }
            else
            {
                ResultTarget->Tracker.reset();
            }
        }

        //============================================================
        // Detect new possible units.
        //============================================================
        auto objects = Detector->Detect(SourceVision->Picture,
                                        static_cast<float>(ConfidenceThreshold),
                                        0.9);

        auto& source_picture = SourceVision->Picture;
        auto& result_picture = ResultVision->Picture;

        Gaia::DNNHelper::YoloV4::Object* best_object = nullptr;
        int best_object_score = -1;

        //============================================================
        // Check: continue if found suspicious objects, or return.
        //============================================================
        // Condition: No suspicious objects found. Return.
        //------------------------------------------------------------
        if (objects.empty())
        {
            ResultTarget->FoundUnit = false;
            return;
        }
        //------------------------------------------------------------
        // Condition: Found suspicious objects. Continue to find the best object.
        //------------------------------------------------------------
        for (auto& object : objects)
        {
            auto box = object.BoundingBox;

            if (box.width * box.height > 0)
            {
                //------------------------------
                // Modify the interested area.
                //------------------------------
                if (box.x <= 0) box.x = 0;
                if (box.x >= source_picture.cols) box.x = source_picture.cols - 1;
                if (box.y <= 0) box.y = 0;
                if (box.y >= source_picture.rows) box.y = source_picture.rows - 1;
                if (box.x + box.width >= source_picture.cols) box.width = source_picture.cols - box.x;
                if (box.y + box.height >= source_picture.rows) box.height = source_picture.rows - box.y;
                //------------------------------
                // Compare to find the 'best target'.
                //------------------------------
                if (box.width * box.height > best_object_score)
                {
                    best_object = &object;
                }
            }
            //------------------------------
            // Draw targets on result vision.
            //------------------------------
            if (EnableResultHighlighting)
            {
                cv::rectangle(ResultVision->Picture, object.BoundingBox,
                              cv::Scalar(152, 255, 152),3);
                cv::putText(ResultVision->Picture, std::to_string(object.ClassID),
                            cv::Point(object.BoundingBox.x, object.BoundingBox.y - 30),
                            cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(152, 255, 152), 1);
            }
        }

        //============================================================
        // Track and update to the result target.
        //============================================================
        auto& best_target_box = best_object->BoundingBox;
        ResultTarget->Tracker = cv::TrackerKCF::create();
        ResultTarget->Tracker->init(source_picture, best_target_box);
        ResultTarget->TrackerTimestamp.Renew();
        ResultTarget->BoundingBox = best_target_box;
        ResultTarget->Picture = source_picture;
        ResultTarget->FoundUnit = true;
    }
}
