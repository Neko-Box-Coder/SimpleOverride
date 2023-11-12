#ifndef SO_NON_COMPARABLE_COPYABLE_HPP
#define SO_NON_COMPARABLE_COPYABLE_HPP

#include <ostream>

namespace SimpleOverride
{
    template<typename T>
    struct NonComparableCopyable
    {
        T* ReferenceVar = nullptr;

        NonComparableCopyable(){}
        NonComparableCopyable(T& referenceVar) { ReferenceVar = &referenceVar; }
        
        inline bool operator== (NonComparableCopyable<T>& other)
        {
            return true;
        }
        
        inline bool operator!= (NonComparableCopyable& other)
        {
            return false;
        }

        friend std::ostream& operator<<(std::ostream& os, const NonComparableCopyable& other)
        {
            os << "NonComparableCopyable";
            return os;
        }
    };
}

#endif