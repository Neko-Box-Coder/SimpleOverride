#ifndef SO_INTERNAL_ARGUMENTS_DATA_HPP
#define SO_INTERNAL_ARGUMENTS_DATA_HPP

#include "./Internal_ConditionInfo.hpp"
#include "./Internal_ActionInfo.hpp"
#include "./Internal_DataInfo.hpp"

namespace SimpleOverride
{
    struct Internal_ArgumentsData
    {
        Internal_ConditionInfo ArgumentsConditionInfo;
        std::vector<Internal_DataInfo> ArgumentsDataInfo;
        Internal_ActionInfo ArgumentsActionInfo;
    };
}

#endif