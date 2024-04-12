#pragma once

#include <EitrFramework/EitrFramework.hpp>
#include <Wt/WContainerWidget.h>
#include <Wt/WTableView.h>
#include <Wt/WStandardItemModel.h>
#include <memory>

namespace Ymir
{
    class ControlCenterService;

    class LogView : public Wt::WContainerWidget
    {
    protected:
        ControlCenterService* Host;

        std::shared_ptr<Wt::WStandardItemModel> Data;
        Wt::WTableView* Table;

    public:
        explicit LogView(ControlCenterService* host);

        void DisplayLog(const std::string& file_name);
    };
}