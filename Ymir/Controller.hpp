#pragma once

#include <memory>
#include <list>
#include <type_traits>

#include <EitrFramework/EitrFramework.hpp>

namespace Ymir
{
    /**
     * @brief Controller controls the whole life circle and services of the system.
     */
    class Controller : public Eitr::Application
    {
    protected:
        bool OnInstall() override;

        void OnUninstall() override;

        bool OnUpdate() override;
    };
}