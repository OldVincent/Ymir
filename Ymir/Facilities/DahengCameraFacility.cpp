#include "DahengCameraFacility.hpp"

#include <GaiaExceptions/GaiaExceptions.hpp>

namespace Ymir
{
    using namespace Eitr;

    /// Install the facility and open the camera.
    bool DahengCameraFacility::OnInstall(const boost::json::object &configuration)
    {
        auto camera_index = configuration.at("Index").as_int64();

        auto camera_install_success = false;
        while (!camera_install_success)
        {
            try
            {
                Device = std::dynamic_pointer_cast<Gaia::Cameras::Daheng::Camera>(
                        Gaia::Cameras::Daheng::Factory::GetInstance()->GetCamera(camera_index));
                camera_install_success = true;
            }
            catch(Gaia::Exceptions::ExceptionWrapper<std::runtime_error>& error)
            {
                GetRuntime()->Log.Record(Name,
                                         "Can not open camera with index of " + std::to_string(camera_index) +
                                         ", will try in 1 second:" + error.what(),
                                         LogRecorder::Level::Warning);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        GetRuntime()->Log.Record(Name,
                                 "Camera index " + std::to_string(camera_index) + " online.",
                                 LogRecorder::Level::Milestone);

        ApplyOption(configuration, "Exposure", [this](const boost::json::value& value){
            return Device->SetExposureTime(std::chrono::microseconds(value.as_int64()));
        }, true);


        ApplyOption(configuration, "Gain", [this](const boost::json::value& value){
            return Device->SetGain(value.as_int64());
        }, true);


        ApplyOption(configuration, "BalanceRed", [this](const boost::json::value& value){
            return Device->SetWhiteBalanceRedValue(value.as_double());
        }, true);

        ApplyOption(configuration, "BalanceGreen", [this](const boost::json::value& value){
            return Device->SetWhiteBalanceGreenValue(value.as_double());
        }, true);

        ApplyOption(configuration, "BalanceBlue", [this](const boost::json::value& value){
            return Device->SetWhiteBalanceBlueValue(value.as_double());
        }, true);

        return true;
    }

    /// Close the camera and uninstall the facility.
    void DahengCameraFacility::OnUninstall()
    {
        Device.reset();
    }

    /// Get the device pointer of the opened camera.
    Gaia::Cameras::Daheng::Camera *DahengCameraFacility::GetCamera()
    {
        return Device.get();
    }
}
