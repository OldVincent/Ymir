#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <GaiaCameraInterface/GaiaCameraInterface.hpp>
#include <GaiaCameraForDaheng/GaiaCameraForDaheng.hpp>

#include "../Models/VisionModel.hpp"
#include "../Facilities/DahengCameraFacility.hpp"

namespace Ymir
{
    DECLARE_SERVICE(DahengCameraReader);

    /**
     * @brief Service for auto grab pictures from Daheng industrial camera.
     * @details
     *  Its configuration file should contains:
     *  ~ Index (int);
     *  ~ Exposure (double, optional);
     *  ~ Gain (int, optional);
     *  ~ PictureOvertime (int, optional, milliseconds);
     *  It will maintain a VisionModel of the same name of this service.
     */
    class DahengCameraReaderService : public Eitr::Service
    {
    protected:
        /// The facility of the Daheng camera.
        DahengCameraFacility* Camera {nullptr};
        /// The model of the camera data.
        VisionModel* Vision {nullptr};

        /**
         * @brief The wanted size of the picture.
         * @details
         *  Set 0 to a dimension will keep the original size of this dimension.
         */
        cv::Size TargetSize {0,0};

        /// The method this service use to resize the original picture into the TargetSize.
        enum class ResizeMethodType
        {
            None,
            Resize,
            Crop
        }ResizeMethod {ResizeMethodType::None};

        /// The timestamp of the last picture retrieved.
        std::chrono::steady_clock::time_point LastPictureTimestamp;

        /**
         * @brief The overtime milliseconds of the timestamp of the picture.
         * @details If the picture has not been updated after this amount of milliseconds, the camera will be considered
         *          to be offline.
         */
        long TimestampOvertimeMilliseconds {-1};

        /// Install this service and open the camera.
        bool OnInstall(const boost::json::object &configuration) override;
        /// Uninstall this service and close the camera.
        void OnUninstall() override;
        /// Retrieve the latest picture and
        void OnUpdate() override;
    };
}