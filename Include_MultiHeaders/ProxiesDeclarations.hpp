#ifndef SO_PROXIES_DECLARATIONS_HPP
#define SO_PROXIES_DECLARATIONS_HPP

#include "./ProxyType.hpp"

#include <functional>
#include <string>
#include <vector>

namespace SimpleOverride
{
    class Overrider;
    
    //Common proxy class for method chaining
    template<typename DeriveType>
    class CommonProxy
    {
        friend class Overrider;
        friend class Internal_ReturnDataSetter;
        friend class Internal_ArgsDataSetter;
        friend class Internal_RequirementSetter;
        
        protected:
            std::string FunctionSignatureName;
            Overrider& SimpleOverrideObj;
            const ProxyType FunctionProxyType;

        public:
            CommonProxy(std::string functionSignatureName, 
                        Overrider& SimpleOverrideObj, 
                        ProxyType functionProxyType) :  FunctionSignatureName(functionSignatureName),
                                                        SimpleOverrideObj(SimpleOverrideObj),
                                                        FunctionProxyType(functionProxyType)
            {}

            DeriveType& Times(int times);
            
            template<typename... Args>
            DeriveType& WhenCalledWith(Args... args);
            
            DeriveType& If(std::function<bool(const std::vector<void*>& args)> condition);

            DeriveType& Otherwise_Do(std::function<void(const std::vector<void*>& args)> action);

            DeriveType& WhenCalledExpectedly_Do(std::function<void(const std::vector<void*>& args)> action);
    };

    //Override return proxy class for method chaining
    class ReturnProxy : public CommonProxy<ReturnProxy>
    {
        public:
            ReturnProxy(std::string functionSignatureName, 
                        Overrider& SimpleOverrideObj, 
                        ProxyType functionProxyType) : CommonProxy( functionSignatureName, 
                                                                    SimpleOverrideObj, 
                                                                    functionProxyType) 
            {}
            
            template<typename T>
            ReturnProxy& ReturnsByAction(std::function<void(const std::vector<void*>& args, 
                                                            void* out)> returnAction);
            
            template<typename T>
            ReturnProxy& Returns(T returnData);
    };

    //Override arguments proxy class for method chaining
    class ArgumentsProxy : public CommonProxy<ArgumentsProxy>
    {
        public:
            ArgumentsProxy( std::string functionSignatureName, 
                            Overrider& SimpleOverrideObj, 
                            ProxyType functionProxyType) :  CommonProxy(functionSignatureName, 
                                                            SimpleOverrideObj, 
                                                            functionProxyType) 
            {}
            
            template<typename T>
            ArgumentsProxy& SetArgByAction(std::function<void( const std::vector<void*>& args, 
                                                                void* currentArg)> setArgsAction);
            
            template<typename... Args>
            ArgumentsProxy& SetArgs(Args... args);
    };
}

#endif