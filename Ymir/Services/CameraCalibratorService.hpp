#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <GaiaCameraForDaheng/GaiaCameraForDaheng.hpp>

#include "../Facilities/DahengCameraFacility.hpp"
#include "../Orders/CameraSettingOrder.hpp"

namespace Ymir
{
    DECLARE_SERVICE(CameraCalibrator)
    /**
     * @brief Create a visual calibrator window for camera calibration.
     */
    class CameraCalibratorService : public Eitr::Service
    {
    protected:
        int LastExposure {-1};
        int LastGain {-1};
        int LastWhiteBalanceRed {-1};
        int LastWhiteBalanceBlue {-1};

        DahengCameraFacility* CameraFacility;
        CameraSettingOrder* SettingOrder;

        bool OnInstall(const boost::json::object &configuration) override;

        void OnUninstall() override;

        void OnUpdate() override;

    public:
        void SetSettings(int exposure, int gain, int white_balance_red, int white_balance_blue);
    };
}