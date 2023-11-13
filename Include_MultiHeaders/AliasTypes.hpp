#ifndef SO_ALIAS_TYPES_HPP
#define SO_ALIAS_TYPES_HPP

#include "./Any.hpp"
#include "./NonComparable.hpp"
#include "./NonComparableCopyable.hpp"
#include "./NonCopyable.hpp"

const SimpleOverride::Any SO_ANY;
const SimpleOverride::Any SO_DONT_SET;
const SimpleOverride::Any SO_DONT_OVERRIDE_RETURN;

template<typename T>
using SO_NonCopyable = SimpleOverride::NonCopyable<T>;
    
template<typename T>
using SO_NonComparable = SimpleOverride::NonComparable<T>;

template<typename T>
using SO_NonComparableCopyable = SimpleOverride::NonComparableCopyable<T>;

#endif