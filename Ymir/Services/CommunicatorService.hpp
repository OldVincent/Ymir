#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include "../Modules/Attack.pb.hpp"
#include "../Models/TargetModel.hpp"
#include <GaiaByteUtility/GaiaByteUtility.hpp>
#include <GaiaSerialIO/SerialIO.hpp>
#include <memory>

namespace Ymir
{
    DECLARE_SERVICE(Communicator);

    class CommunicatorService : public Eitr::Service
    {
    protected:

        static constexpr double PI = 3.1415926;

        std::unique_ptr<Gaia::SerialIO::SerialPort> Port;

        TargetModel* Target;

        double VerticalFOV;
        double HorizontalFOV;

        bool OnInstall(const boost::json::object &configuration) override;

        void OnUninstall() override;

        void OnUpdate() override;
    };
}