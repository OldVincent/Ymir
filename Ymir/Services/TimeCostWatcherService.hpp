#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include "../Models/VisionModel.hpp"

namespace Ymir
{
    DECLARE_SERVICE(TimeCostWatcher);

    /**
     * @brief This service will display the frame time cost to the console or a vision model.
     * @details
     *  Its configuration contains:
     *  ~ TargetType (string, "Console" or "Picture");
     *  ~ TargetPicture (string, if TargetType is "Picture".);
     *  ~ TextPosition (array of int[2], optional);
     *  ~ TextColor (array of int[3], optional, BGR format.);
     *  ~ TextThickness (int, optional);
     *  ~ TextScale (double, optional);
     */
    class TimeCostWatcherService : public Eitr::Service
    {
    public:
        /**
         * @brief The target to display FPS.
         * @details
         *  Console: print the frame time cost to the console.
         *  Picture: render the frame time cost to a VisionModel.
         */
        enum DisplayTargetType
        {
            Console,
            Picture
        }DisplayTarget {DisplayTargetType::Console};

        /// The picture to render FPS text.
        VisionModel* TargetPicture;

        /// Set the position of the text in the picture.
        cv::Point TextPosition {30, 30};
        /// Set the color of the text in the picture.
        cv::Scalar TextColor {112, 25, 25};
        /// Set the size of the text in the picture.
        double TextScale {1.0};
        /// Set the thickness of the text in the picture.
        int TextThickness {2};

    private:
        /// Install this service.
        bool OnInstall(const boost::json::object &configuration) override;
        /// Uninstall this service.
        void OnUninstall() override;
        /// Display the time cost of the last update.
        void OnUpdate() override;
    };
}