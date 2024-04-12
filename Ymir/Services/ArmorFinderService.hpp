#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include "../Models/TargetModel.hpp"
#include "../Models/VisionModel.hpp"

namespace Ymir
{
    DECLARE_SERVICE(ArmorFinder);

    class ArmorFinderService : public Eitr::Service
    {
    public:
        bool EnableResultVision {false};

    protected:
        TargetModel* Target;
        VisionModel* ResultVision;

        unsigned int LightBarThreshold {250};

        bool OnInstall(const boost::json::object &configuration) override;

        void OnUninstall() override;

        void OnUpdate() override;
    };
}