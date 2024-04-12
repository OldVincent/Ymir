#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTextArea.h>
#include <Wt/WComboBox.h>

#include <string>

namespace Ymir
{
    class ControlCenterService;

    class ConfigurationView : public Wt::WContainerWidget
    {
    protected:
        ControlCenterService* Host;

        Wt::WTextArea* TextEditor;

        std::string OpenedConfigurationPath;

    public:
        explicit ConfigurationView(ControlCenterService* host);

        void OpenConfiguration(const std::string& path);

        void SaveConfiguration();
    };
}
