#ifndef SO_INTERNAL_OVERRIDE_ARGUMENT_INFO_HPP
#define SO_INTERNAL_OVERRIDE_ARGUMENT_INFO_HPP

#include "./Internal_ArgsData.hpp"
#include <vector>

namespace SimpleOverride
{
    struct Internal_OverrideArgumentInfo
    {
        std::vector<Internal_ArgsData> ArgumentsDatas = {};
    };
}

#endif