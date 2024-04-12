#include "ArmorFinderService.hpp"

#include <tbb/tbb.h>
#include "../Modules/GeometryFeatureModule.hpp"
#include "../Modules/ImageDebugUtility.hpp"

namespace Ymir
{

    bool ArmorFinderService::OnInstall(const boost::json::object &configuration)
    {
        ApplyOption(configuration, "Target",[this](const boost::json::value& value){
            Target = GetApplication()->GetModel<TargetModel>(value.as_string().c_str());
            return true;
        });

        ApplyOption(configuration, "LightBarThreshold",[this](const boost::json::value& value){
            LightBarThreshold = value.as_int64();
            return true;
        }, true);

        EnableResultVision = ApplyOption(configuration, "ResultVision", [this](const boost::json::value& value){
            ResultVision = GetApplication()->GetModel<VisionModel>(value.as_string().c_str());
            return true;
        }, true);
        return true;
    }

    void ArmorFinderService::OnUninstall()
    {}

    void ArmorFinderService::OnUpdate()
    {
        if (!Target->FoundUnit)
        {
            Target->FoundArmor = false;
            return;
        };

        cv::Mat hsv_picture;
        cv::cvtColor(Target->Picture(Target->BoundingBox), hsv_picture, cv::COLOR_BGR2HSV);

        std::vector<cv::Mat> channels;
        cv::split(hsv_picture, channels);

        auto& h_channel = channels[0];
        auto& s_channel = channels[1];
        auto& v_channel = channels[2];

        cv::Mat v_mask;

        cv::threshold(v_channel, v_mask, LightBarThreshold, 255, cv::THRESH_BINARY);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(v_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        tbb::concurrent_vector<RoboPioneers::Modules::GeometryFeatureModule::GeometryFeature> light_bar_candidates;
        light_bar_candidates.reserve(10);

        tbb::parallel_for_each(contours, [&light_bar_candidates]
                (const std::vector<cv::Point>& contour){

            auto rotated_rectangle = cv::minAreaRect(contour);
            auto geometry_feature =
                    RoboPioneers::Modules::GeometryFeatureModule::StandardizeRotatedRectangle(rotated_rectangle);
            if (geometry_feature.Angle >= 25 && geometry_feature.Angle <= 155)
            {
                light_bar_candidates.emplace_back(geometry_feature);
            }
        });

        tbb::concurrent_vector<std::tuple<RoboPioneers::Modules::GeometryFeatureModule::GeometryFeature,
                RoboPioneers::Modules::GeometryFeatureModule::GeometryFeature>> light_bar_pairs;

        // 串行地制作候选列表
        for (auto first_index = light_bar_candidates.begin(); first_index != light_bar_candidates.end(); ++first_index)
        {
            auto second_index = first_index;
            ++second_index;

            auto first_feature = *first_index;

            for (; second_index != light_bar_candidates.end(); ++second_index)
            {
                auto second_feature = *second_index;

                light_bar_pairs.emplace_back(first_feature, second_feature);
            }
        }

        tbb::concurrent_vector<cv::Rect> armor_candidates;

        // 并行地进行判断
        tbb::parallel_for_each(light_bar_pairs, [area = Target->BoundingBox, &armor_candidates](
                const std::tuple<RoboPioneers::Modules::GeometryFeatureModule::GeometryFeature,
                        RoboPioneers::Modules::GeometryFeatureModule::GeometryFeature>& candidate){

            auto first_geometry_feature = std::get<0>(candidate);
            auto second_geometry_feature = std::get<1>(candidate);

            //------------------------------------------------------------
            // y坐标之差小于最小高度的0.5
            //------------------------------------------------------------
            auto y_difference = first_geometry_feature.Center.y - second_geometry_feature.Center.y;
            if (y_difference < 0) y_difference = - y_difference;
            auto light_bar_smaller_height = first_geometry_feature.Length < second_geometry_feature.Length ?
                                            first_geometry_feature.Length : second_geometry_feature.Length;
            if (static_cast<double>(y_difference) * 2.0f > static_cast<double>(light_bar_smaller_height))
                return;

            //------------------------------------------------------------
            // 角度之差小于等于30度，即近似平行
            //------------------------------------------------------------
            auto angle_difference = first_geometry_feature.Angle - second_geometry_feature.Angle;
            if (angle_difference < 0) angle_difference = - angle_difference;
            if (angle_difference > 30) return;

            //------------------------------------------------------------
            // 排除内八程度严重的匹配
            //------------------------------------------------------------
            if ((first_geometry_feature.Angle - 90)*(second_geometry_feature.Angle - 90) < 0)
                return;

            //------------------------------------------------------------
            // 高度比 小值比大值大于0.3
            //------------------------------------------------------------
            auto bigger_height = first_geometry_feature.Length > second_geometry_feature.Length ?
                                 first_geometry_feature.Length : second_geometry_feature.Length;
            auto smaller_height = first_geometry_feature.Length <= second_geometry_feature.Length ?
                                  first_geometry_feature.Length : second_geometry_feature.Length;
            auto height_ratio = static_cast<float>(smaller_height) / static_cast<float>(bigger_height);
            if (height_ratio < 0.2) return;

            //------------------------------------------------------------
            // 高度差-高度比 高度差比高度小值小于等于1.5
            //------------------------------------------------------------
            auto height = first_geometry_feature.Length < second_geometry_feature.Length ?
                          first_geometry_feature.Length : second_geometry_feature.Length;

            auto delta_y = first_geometry_feature.Center.y - second_geometry_feature.Center.y;
            if (delta_y < 0) delta_y = - delta_y;

            auto delta_y_height_ratio = static_cast<float>(delta_y) / static_cast<float>(height);
            if (delta_y_height_ratio > 1.5)
                return;

            //------------------------------------------------------------
            // 重新生成外接框
            //------------------------------------------------------------
            std::vector<cv::Point2f> vertices;
            vertices.resize(8);
            first_geometry_feature.Raw.CircumscribedRectangle.points(&vertices[0]);
            second_geometry_feature.Raw.CircumscribedRectangle.points(&vertices[4]);
            auto armor_candidate = cv::boundingRect(vertices);

            if (armor_candidate.width > static_cast<double>(area.width) * 0.5f)
                return;
            if (static_cast<double>(armor_candidate.width) / static_cast<double>(armor_candidate.height) > 4.6f)
                return;
            if (static_cast<double>(armor_candidate.width) / static_cast<double>(area.width) > 0.5f)
                return;
            armor_candidates.emplace_back(armor_candidate);
        });

        cv::Rect* most_central_armor = nullptr;
        int most_central_armor_distance;

        if (armor_candidates.empty())
        {
            Target->FoundArmor = false;
            return;
        }

        for (auto& rectangle : armor_candidates)
        {
            cv::Rect global_rectangle = rectangle;
            global_rectangle.x += Target->BoundingBox.x;
            global_rectangle.y += Target->BoundingBox.y;

            int x_factor = rectangle.x - rectangle.width / 2;
            int y_factor = rectangle.y - rectangle.height / 2;
            int current_armor_distance = x_factor * x_factor + y_factor * y_factor;
            if (!most_central_armor || most_central_armor_distance > current_armor_distance)
            {
                most_central_armor = &rectangle;
                most_central_armor_distance = current_armor_distance;
            }

            if (EnableResultVision)
            {
                cv::rectangle(ResultVision->Picture, global_rectangle,
                              cv::Scalar(0, 165, 255),1);
            }
        }

        Target->ArmorCenterX = Target->BoundingBox.x + most_central_armor->x + most_central_armor->width / 2;
        Target->ArmorCenterY = Target->BoundingBox.y + most_central_armor->y + most_central_armor->height / 2;

        if (EnableResultVision)
        {
            cv::circle(ResultVision->Picture, cv::Point(
                        static_cast<int>(Target->ArmorCenterX), static_cast<int>(Target->ArmorCenterY)),
                       2, cv::Scalar(0, 255, 0), 2);
        }
        Target->FoundArmor = true;
    }
}
