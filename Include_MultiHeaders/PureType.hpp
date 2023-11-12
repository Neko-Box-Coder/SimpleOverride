#ifndef SO_PURE_TYPE_HPP
#define SO_PURE_TYPE_HPP

#include <type_traits>

namespace SimpleOverride
{
    #define INTERNAL_SO_UNCONST(targetType) typename std::remove_const<targetType>::type
    #define INTERNAL_SO_PURE_T INTERNAL_SO_UNCONST(T)
}

#endif