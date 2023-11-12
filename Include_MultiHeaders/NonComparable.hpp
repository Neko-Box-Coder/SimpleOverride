#ifndef SO_NON_COMPARABLE_HPP
#define SO_NON_COMPARABLE_HPP

#include <ostream>

namespace SimpleOverride
{
    template<typename T>
    struct NonComparable
    {
        T* ReferenceVar = nullptr;

        NonComparable(){}
        NonComparable(T& referenceVar) { ReferenceVar = &referenceVar; }
        
        inline bool operator== (NonComparable<T>& other)
        {
            return true;
        }
        
        inline bool operator!= (NonComparable& other)
        {
            return false;
        }

        friend std::ostream& operator<<(std::ostream& os, const NonComparable& other)
        {
            os << "NonComparable";
            return os;
        }
    };
}

#endif