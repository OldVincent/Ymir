#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <GaiaDNNHelper/GaiaDNNHelper.hpp>
#include <memory>
#include "../Models/VisionModel.hpp"
#include "../Models/TargetModel.hpp"
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/tracking.hpp>

namespace Ymir
{
    DECLARE_SERVICE(UnitDetector);

    /**
     * @brief Service that will use Yolo to detect object in the picture.
     * @details
     *  Its configuration should contains:
     *  ~ Model (string, consists of the directory path of the model and the name of the model,
     *  the path of .weights and .cfg file will be generated according to this value, for example:
     *  "./YoloV4" means this service will use "./YoloV4.weights" and "./YoloV4.cfg").
     *  ~ Width (int, the width of the matrix that the input layer of the network accepts).
     *  ~ Height (int, the height of the matrix that the input layer of the network accepts.)
     *  ~ Threshold (double, optional, the threshold of confidence, default value is 0.7.)
     *  ~ ResultName (string, optional, the name of the UnitCandidateOrder to store the detected objects, the default value is
     *    the name of this service.)
     *  ~ VisionSource (string, the name of the VisionModel to get picture to detect.)
     *  ~ VisionResult (string, optional, the name of the VisionModel to display the result.)
     */
    class UnitDetectorService : public Eitr::Service
    {
    public:
        /**
         * @brief The minimum confidence of objects to accept.
         * @details
         *  Only detected objects with the confidence higher than this will be accepted.
         */
        double ConfidenceThreshold {0.7f};

    protected:
        /// Whether paint bounding box to a picture or not.
        bool EnableResultHighlighting {false};

        VisionModel* SourceVision {nullptr};

        /// The picture to paint bounding boxes.
        VisionModel* ResultVision {nullptr};

        /// The main target.
        TargetModel* ResultTarget {nullptr};

        /// The detector of YoloV4 network.
        std::unique_ptr<Gaia::DNNHelper::YoloV4> Detector;

        /// Install and load the network.
        bool OnInstall(const boost::json::object &configuration) override;
        /// Uninstall.
        void OnUninstall() override;
        /// Use the network to detect objects in the picture.
        void OnUpdate() override;
    };
}