#pragma once

#include <EitrFramework/EitrFramework.hpp>

namespace Ymir
{
    class CameraSettingOrder : public Eitr::Order
    {
    public:
        int LastExposure {-1};
        int LastGain {-1};
        int LastWhiteBalanceRed {-1};
        int LastWhiteBalanceBlue {-1};

        int Exposure {-1};
        int Gain {-1};
        int WhiteBalanceRed {-1};
        int WhiteBalanceBlue {-1};

        void Clear() override
        {
            if (Exposure >= 0) LastExposure = Exposure;
            if (Gain >= 0) LastGain = Gain;
            if (WhiteBalanceRed >= 0) LastWhiteBalanceRed = WhiteBalanceRed;
            if (WhiteBalanceBlue >= 0) LastWhiteBalanceBlue = WhiteBalanceBlue;

            Exposure = -1;
            Gain = -1;
            WhiteBalanceBlue = -1;
            WhiteBalanceRed = -1;
        }

        bool IsEmpty() override
        {
            return (Exposure < 0 && Gain < 0 && WhiteBalanceRed < 0 && WhiteBalanceBlue < 0);
        }
    };
}