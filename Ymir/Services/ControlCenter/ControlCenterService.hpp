#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <thread>
#include <Wt/WServer.h>
#include <Wt/WApplication.h>
#include "MainPanel.hpp"

namespace Ymir
{
    DECLARE_SERVICE(ControlCenter);

    class ControlCenterService : public Eitr::Service
    {
    protected:
        /// Web server.
        std::unique_ptr<Wt::WServer> WebServer;

        /// Install and start up the web server.
        bool OnInstall(const boost::json::object &configuration) override;
        /// Uninstall and stop the web server.
        void OnUninstall() override;

        void OnUpdate() override;

    public:
        /// Control panel web application.
        MainPanel* WebPanel;
    };
}