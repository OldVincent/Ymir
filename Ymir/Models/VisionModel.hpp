#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <tuple>
#include <chrono>
#include <atomic>

namespace Ymir
{
    /**
     * @brief Vision model represents vision data gotten from a camera or other vision sensors.
     */
    class VisionModel : public Eitr::Model
    {
    public:
        /// The whole picture of the vision, with the color format of BGR.
        cv::Mat Picture;
        /// The time point when this picture is retrieved.
        std::chrono::steady_clock::time_point Timestamp;
    };
}