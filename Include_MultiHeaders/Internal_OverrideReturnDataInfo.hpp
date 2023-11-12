#ifndef SO_INTERNAL_OVERRIDE_RETURN_DATA_INFO_HPP
#define SO_INTERNAL_OVERRIDE_RETURN_DATA_INFO_HPP

#include <vector>
#include "./Internal_ReturnData.hpp"

namespace SimpleOverride
{
    struct Internal_OverrideReturnDataInfo
    {
        std::vector<Internal_ReturnData> ReturnDatas = {};
    };
}

#endif