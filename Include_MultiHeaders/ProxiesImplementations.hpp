#ifndef SO_PROXIES_IMPLEMENTATIONS_HPP
#define SO_PROXIES_IMPLEMENTATIONS_HPP

#include "./SimpleOverrideInstance.hpp"
#include "./ProxiesDeclarations.hpp"

namespace SimpleOverride
{
    //==============================================================================
    //Implementation of proxy classes for method chaining
    //==============================================================================

    //NOTE: Can't do using as this will evaluate the DeriveType which contains 
    //      SimpleOverrideCommonProxy which has functions not defined yet.
    //      See https://stackoverflow.com/questions/35428422/crtp-accessing-incomplete-type-members
    //template<typename DeriveType>
    //using CommonProxy = SimpleOverrideCommonProxy<DeriveType>;

    template<typename T>
    inline ReturnProxy& ReturnProxy::ReturnsByAction(std::function<void(std::vector<void*>& args, 
                                                                        void* out)> returnAction)
    {
        return SimpleOverrideObj.ReturnsByAction<T>(*this, returnAction);
    }

    template<typename T>
    inline ReturnProxy& ReturnProxy::Returns(T returnData)
    {
        return SimpleOverrideObj.Returns(*this, returnData);
    }

    template<typename T>
    inline ArgumentsProxy& 
        ArgumentsProxy::SetArgsByAction(std::function<void( std::vector<void*>& args, 
                                                            void* out)> setArgsAction)
    {
        return SimpleOverrideObj.SetArgsByAction<T>(*this, setArgsAction);
    }
            
    template<typename... Args>
    inline ArgumentsProxy& ArgumentsProxy::SetArgs(Args... args)
    {
        return SimpleOverrideObj.SetArgs(*this, args...);
    }

    template<typename DeriveType>
    inline DeriveType& CommonProxy<DeriveType>::Times(int times)
    {
        return SimpleOverrideObj.Times(*this, times);
    }

    template<typename DeriveType>
    template<typename... Args>
    inline DeriveType& CommonProxy<DeriveType>::WhenCalledWith(Args... args)
    {
        return SimpleOverrideObj.WhenCalledWith(*this, args...);
    }

    template<typename DeriveType>
    inline DeriveType& 
        CommonProxy<DeriveType>::If(std::function<bool(std::vector<void*>& args)> condition)
    {
        return SimpleOverrideObj.If(*this, condition);
    }

    template<typename DeriveType>
    inline DeriveType& 
        CommonProxy<DeriveType>::Otherwise_Do(std::function<void(std::vector<void*>& args)> action)
    {
        return SimpleOverrideObj.Otherwise_Do(*this, action);
    }

    template<typename DeriveType>
    inline DeriveType& 
        CommonProxy<DeriveType>::WhenCalledExpectedly_Do(std::function<void(std::vector<void*>& args)> action)
    {
        return SimpleOverrideObj.WhenCalledExpectedly_Do(*this, action);
    }
}

#endif