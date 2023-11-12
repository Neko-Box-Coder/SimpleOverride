#ifndef SO_INTERNAL_RETURN_DATA_HPP
#define SO_INTERNAL_RETURN_DATA_HPP

#include "./Internal_ConditionInfo.hpp"
#include "./Internal_DataInfo.hpp"
#include "./Internal_ActionInfo.hpp"

namespace SimpleOverride
{
    struct Internal_ReturnData
    {
        Internal_ConditionInfo ReturnConditionInfo;
        Internal_DataInfo ReturnDataInfo;
        Internal_ActionInfo ReturnActionInfo;
    };
}

#endif