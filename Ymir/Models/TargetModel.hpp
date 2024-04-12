#pragma once

#include <EitrFramework/EitrFramework.hpp>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/tracking.hpp>

namespace Ymir
{
    class TargetModel : public Eitr::Model
    {
    public:
        bool FoundUnit = false;
        bool FoundArmor = false;

        Eitr::Timestamp TrackerTimestamp {std::chrono::milliseconds(300)};
        cv::Ptr<cv::Tracker> Tracker;
        cv::Rect BoundingBox;
        /// The picture of the whole scene.
        cv::Mat Picture;

        unsigned int ArmorCenterX;
        unsigned int ArmorCenterY;

        void Reset() override
        {
            Tracker.reset();
            FoundUnit = false;
            FoundArmor = false;
            ArmorCenterX = 0;
            ArmorCenterY = 0;
        }
    };
}