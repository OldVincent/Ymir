#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include "../Models/VisionModel.hpp"

namespace Ymir
{
    DECLARE_SERVICE(VisionViewer);

    /**
     * @brief This service can open a window to display vision data.
     * @details
     *  Its configuration should contains:
     *  ~ Vision (string, the name of the vision model.)
     */
    class VisionViewerService : public Eitr::Service
    {
    protected:
        /// The picture to show.
        VisionModel* Vision {nullptr};

        /// Install and create the window.
        bool OnInstall(const boost::json::object &configuration) override;
        /// Uninstall and destroy the window.
        void OnUninstall() override;
        /// Update the picture on the window.
        void OnUpdate() override;
    };
}