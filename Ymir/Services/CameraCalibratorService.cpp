#include "CameraCalibratorService.hpp"

#include <opencv4/opencv2/opencv.hpp>

namespace Ymir
{
    bool CameraCalibratorService::OnInstall(const boost::json::object &configuration)
    {
        ApplyOption(configuration, "Camera", [this](const boost::json::value& value){
            CameraFacility = GetApplication()->GetFacility<DahengCameraFacility>(value.as_string().c_str());
            return true;
        });

        ApplyOption(configuration, "Order", [this](const boost::json::value& value){
            SettingOrder = GetApplication()->GetOrder<CameraSettingOrder>(value.as_string().c_str());
            return true;
        });

        if (!CameraFacility)
        {
            Eitr::GetRuntime()->Log.Record(Name, "Camera Facility is null.", Eitr::LogRecorder::Level::Error);
            return false;
        }
        if (!SettingOrder)
        {
            Eitr::GetRuntime()->Log.Record(Name, "Setting DetectionOrder is null.", Eitr::LogRecorder::Level::Error);
            return false;
        }

        int exposure = -1;
        ApplyOption(configuration, "SavedExposure", [this, &exposure](const boost::json::value& value){
            exposure = static_cast<int>(value.as_int64());
            return true;
        }, true);

        int gain = -1;
        ApplyOption(configuration, "SavedGain", [this, &gain](const boost::json::value& value){
            gain = static_cast<int>(value.as_int64());
            return true;
        }, true);

        int balance_red = -1;
        ApplyOption(configuration, "SavedWhiteBalanceRed", [this, &balance_red]
            (const boost::json::value& value){
            balance_red = static_cast<int>(value.as_int64());
            return true;
        }, true);

        int balance_blue = -1;
        ApplyOption(configuration, "SavedWhiteBalanceBlue", [this, &balance_blue]
            (const boost::json::value& value){
            balance_blue = static_cast<int>(value.as_int64());
            return true;
        }, true);
        SetSettings(exposure, gain, balance_red, balance_blue);
        SettingOrder->LastExposure = exposure;
        SettingOrder->LastGain = gain;
        SettingOrder->LastWhiteBalanceRed = balance_red;
        SettingOrder->LastWhiteBalanceBlue = balance_blue;
        return true;
    }

    void CameraCalibratorService::OnUninstall()
    {
        auto configuration = LoadConfiguration();
        configuration.insert_or_assign("SavedExposure", LastExposure);
        configuration.insert_or_assign("SavedGain", LastGain);
        configuration.insert_or_assign("SavedWhiteBalanceRed", LastWhiteBalanceRed);
        configuration.insert_or_assign("SavedWhiteBalanceBlue", LastWhiteBalanceBlue);
        SaveConfiguration(configuration);
    }

    void CameraCalibratorService::OnUpdate()
    {
        if (SettingOrder)
        {
            SetSettings(SettingOrder->Exposure, SettingOrder->Gain,
                        SettingOrder->WhiteBalanceRed, SettingOrder->WhiteBalanceBlue);
        }
    }

    void CameraCalibratorService::SetSettings(int exposure, int gain, int white_balance_red, int white_balance_blue)
    {
        if (exposure >= 0 && LastExposure != exposure)
        {
            if (CameraFacility->GetCamera()->SetExposureTime(std::chrono::microseconds(exposure)))
            {
                LastExposure = exposure;
            }
        }
        if (gain >= 0 && LastGain != gain)
        {
            if (CameraFacility->GetCamera()->SetGain(gain))
            {
                LastGain = gain;
            }
        }
        if (white_balance_red >= 0 && LastWhiteBalanceRed != white_balance_red)
        {
            if (CameraFacility->GetCamera()->SetWhiteBalanceRedValue(
                    static_cast<double>(white_balance_red) / 10000.0f))
            {
                LastWhiteBalanceRed = white_balance_red;
            }
        }
        if (white_balance_blue >= 0 && LastWhiteBalanceBlue != white_balance_blue)
        {
            if (CameraFacility->GetCamera()->SetWhiteBalanceBlueValue(
                    static_cast<double>(white_balance_blue) / 10000.0f))
            {
                LastWhiteBalanceBlue = white_balance_blue;
            }
        }
    }
}