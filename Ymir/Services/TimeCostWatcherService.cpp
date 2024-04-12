#include "TimeCostWatcherService.hpp"

#include <sstream>
#include <iostream>

namespace Ymir
{
    /// Install this service.
    bool TimeCostWatcherService::OnInstall(const boost::json::object &configuration)
    {
        ApplyOption(configuration, "TargetType", [this](const boost::json::value& value){
           if (value.as_string() == "Console")
           {
               DisplayTarget = DisplayTargetType::Console;
               return true;
           }
            if (value.as_string() == "Picture")
            {
                DisplayTarget = DisplayTargetType::Picture;
                return true;
            }
            return false;
        }, true);

        if (DisplayTarget == DisplayTargetType::Picture)
        {
            ApplyOption(configuration, "TargetPicture", [this](const boost::json::value& value){
                std::string target_picture_name {value.as_string().c_str()};
                TargetPicture = GetApplication()->GetModel<VisionModel>(target_picture_name);
                return true;
            });
            ApplyOption(configuration, "TextPosition", [this](const boost::json::value& value){
                TextPosition = cv::Point(static_cast<int>(value.as_array().at(0).as_int64()),
                                         static_cast<int>(value.as_array().at(1).as_int64()));
                return true;
            }, true);
            ApplyOption(configuration, "TextThickness", [this](const boost::json::value& value){
                TextThickness = static_cast<int>(value.as_int64());
                return true;
            }, true);
            ApplyOption(configuration, "TextColor", [this](const boost::json::value& value){
                TextColor = cv::Scalar(static_cast<int>(value.as_array().at(0).as_int64()),
                                       static_cast<int>(value.as_array().at(1).as_int64()),
                                               static_cast<int>(value.as_array().at(2).as_int64()));
                return true;
            }, true);
            ApplyOption(configuration, "TextScale", [this](const boost::json::value& value){
                TextScale = value.as_double();
                return true;
            }, true);
        }

        return true;
    }

    /// Uninstall this service.
    void TimeCostWatcherService::OnUninstall()
    {}

    /// Display the time cost of the last update.
    void TimeCostWatcherService::OnUpdate()
    {
        auto last_delta_time = GetApplication()->GetLastUpdateDeltaTime();
        std::stringstream text_builder;
        text_builder << std::setprecision(2);
        text_builder << std::fixed;
        text_builder << last_delta_time;
        text_builder << "ms";

        if (DisplayTarget == DisplayTargetType::Picture)
        {
            cv::putText(TargetPicture->Picture, text_builder.str(), TextPosition,
                        cv::FONT_HERSHEY_COMPLEX, TextScale, TextColor, TextThickness);
        }
        else
        {
            std::cout << "Time cost:" << text_builder.str() << std::endl;
        }

    }
}