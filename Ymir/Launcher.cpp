#include <iostream>

#include "Controller.hpp"
#include <EitrFramework/EitrFramework.hpp>

int main(int arguments_count, char** arguments)
{
    Eitr::GetRuntime()->Initialize(arguments_count, arguments);
    Eitr::GetRuntime()->LaunchApplication<Ymir::Controller>();
}