#ifndef SO_NON_COPYABLE_HPP
#define SO_NON_COPYABLE_HPP

#include <ostream>

namespace SimpleOverride
{
    template<typename T>
    struct NonCopyable
    {
        T* ReferenceVar = nullptr;

        NonCopyable(){}
        NonCopyable(T& referenceVar) { ReferenceVar = &referenceVar; }
        
        inline bool operator== (NonCopyable<T>& other)
        {
            return *ReferenceVar == (*other.ReferenceVar);
        }
        
        inline bool operator!= (NonCopyable& other)
        {
            return !operator==(other);
        }

        friend std::ostream& operator<<(std::ostream& os, const NonCopyable& other)
        {
            os << "NonCopyable";
            return os;
        }
    };
}

#endif