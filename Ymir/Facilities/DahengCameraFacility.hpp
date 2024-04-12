#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <GaiaCameraForDaheng/GaiaCameraForDaheng.hpp>

namespace Ymir
{
    DECLARE_FACILITY(DahengCamera);

    /**
     * @brief Facility for Daheng industrial camera.
     */
    class DahengCameraFacility : public Eitr::Facility
    {
    protected:
        std::shared_ptr<Gaia::Cameras::Daheng::Camera> Device;

        /// The timestamp of the last picture retrieved.
        std::chrono::steady_clock::time_point LastPictureTimestamp;

        /// Install the facility and open the camera.
        bool OnInstall(const boost::json::object &configuration) override;
        /// Close the camera and uninstall the facility.
        void OnUninstall() override;

    public:
        /// Get the device pointer of the opened camera.
        [[nodiscard]] Gaia::Cameras::Daheng::Camera* GetCamera();
    };
}
