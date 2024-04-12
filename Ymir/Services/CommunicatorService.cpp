#include "CommunicatorService.hpp"
#include <cmath>

namespace Ymir
{

    bool CommunicatorService::OnInstall(const boost::json::object &configuration)
    {
        ApplyOption(configuration, "Target", [this](const boost::json::value& value){
            Target = GetApplication()->GetModel<TargetModel>(value.as_string().c_str());
            return true;
        });
        ApplyOption(configuration, "VerticalFOV", [this](const boost::json::value& value){
            VerticalFOV = value.as_double();
            return true;
        });
        ApplyOption(configuration, "HorizontalFOV", [this](const boost::json::value& value){
            HorizontalFOV = value.as_double();
            return true;
        });
        Port = std::make_unique<Gaia::SerialIO::SerialPort>("/dev/ttyTHS2");
        Port->Open();

        return true;
    }

    void CommunicatorService::OnUninstall()
    {
        Port.reset();
    }

    void CommunicatorService::OnUpdate()
    {
        if (Target->FoundArmor)
        {
            Attack message;

            std::string buffer;
            buffer.resize(2);

            *reinterpret_cast<unsigned char*>(&buffer[0]) = 0xBE;
            *reinterpret_cast<unsigned char*>(&buffer[1]) = 0x01;

            auto yaw = std::atan((Target->ArmorCenterY / Target->Picture.rows - 0.5) * tan(VerticalFOV * PI/ 2.0 / 180.0));
            auto pitch = std::atan((Target->ArmorCenterX / Target->Picture.cols - 0.5) * tan(HorizontalFOV * PI/ 2.0 / 180.0));

            message.set_yaw(yaw);
            message.set_pitch(pitch);
            message.set_number(1);
            message.set_command(Attack_CommandType::Attack_CommandType_WAIT);
            message.AppendToString(&buffer);

            Port->Write(Gaia::ByteUtility::BytesAddress(reinterpret_cast<unsigned char*>(buffer.data()), buffer.size()));
        }
    }
}