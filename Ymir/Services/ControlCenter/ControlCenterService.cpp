#include "ControlCenterService.hpp"

namespace Ymir
{
    using namespace Wt;
    using namespace Eitr;

    /// Install and start the web server.
    bool ControlCenterService::OnInstall(const boost::json::object &configuration)
    {
        std::vector<std::string> arguments = {
                "",
                "--http-port", "8080",
                "--http-address", "0.0.0.0",
                "--docroot", "./ControlCenter/WebRoot/",
                "--resources-dir", "./ControlCenter/Resources/"
        };
        WebServer = std::make_unique<Wt::WServer>("./ControlCenter", arguments);
        WebServer->addEntryPoint(EntryPointType::Application, [this](const Wt::WEnvironment& environment){
            return std::make_unique<MainPanel>(environment, this);
        });
        if (!WebServer->start())
        {
            GetRuntime()->Log.Record(Name, "Web server can not be started.", LogRecorder::Level::Warning);
            return false;
        }
        else
        {
            GetRuntime()->Log.Record(Name, "Web server started.", LogRecorder::Level::Milestone);
        }

        return true;
    }

    /// Uninstall and stop the web server.
    void ControlCenterService::OnUninstall()
    {
        WebServer->stop();
        WebServer.reset();
        GetRuntime()->Log.Record(Name, "Web server stopped.", LogRecorder::Level::Milestone);
    }

    void ControlCenterService::OnUpdate()
    {
        Service::OnUpdate();
    }
}