#ifndef SO_INTERNAL_ARGUMENT_DATA_SETTER_HPP
#define SO_INTERNAL_ARGUMENT_DATA_SETTER_HPP

#include "./Internal_OverrideArgumentInfo.hpp"
#include "./ProxiesDeclarations.hpp"
#include "./Any.hpp"
#include "./NonComparable.hpp"
#include "./NonCopyable.hpp"
#include "./NonComparableCopyable.hpp"
#include "./StaticAssertFalse.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

namespace SimpleOverride
{
    class Internal_ArgumentDataSetter
    {
        friend class ArgumentsProxy;
        
        public:
            using ArgumentInfosType = std::unordered_map<std::string, Internal_OverrideArgumentInfo>;
        
        protected:
            ArgumentInfosType& OverrideArgumentsInfos;
            
            template<typename T>
            inline ArgumentsProxy& SetArgsByAction( ArgumentsProxy& proxy,
                                                    std::function<void( std::vector<void*>& args, 
                                                                        void* out)> setArgsAction)
            {
                Internal_ArgumentsData& lastData = 
                    OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back();
                
                lastData.ArgumentsDataInfo.push_back(Internal_DataInfo());
                
                lastData.ArgumentsDataInfo.back().DataAction = setArgsAction;
                lastData.ArgumentsDataInfo.back().DataActionSet = true;
                lastData.ArgumentsDataInfo.back().DataType = typeid(T).hash_code();
                return proxy;
            }
            
            inline ArgumentsProxy& SetArgs(ArgumentsProxy& proxy)
            {
                return proxy;
            }
            
            #define SO_LOG_SetArgs 0
            
            #if 0 | SO_LOG_SetArgs
                #define PRINT_BYTES(val)\
                do\
                {\
                    for(int byteIdx = 0; byteIdx < sizeof(val); byteIdx++)\
                    {\
                        std::cout<<(int)((uint8_t*)&val)[byteIdx] <<", ";\
                    }\
                    std::cout << std::endl;\
                } while(0)
            #endif
            
            template<typename T, typename... Args>
            inline ArgumentsProxy& SetArgs( ArgumentsProxy& proxy,
                                                            T arg, Args... args)
            {
                Internal_ArgumentsData& lastData = 
                    OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back();
                
                lastData.ArgumentsDataInfo.push_back(Internal_DataInfo());
                
                if(!std::is_same<T, Any>())
                {
                    lastData.ArgumentsDataInfo.back().Data = new T(arg);
                    lastData.ArgumentsDataInfo.back().CopyConstructor = 
                        [](void* data) { return new T(*static_cast<T*>(data)); };
                    
                    lastData.ArgumentsDataInfo.back().Destructor = 
                        [](void* data) { delete static_cast<T*>(data); };

                    lastData.ArgumentsDataInfo.back().DataSet = true;
                    lastData.ArgumentsDataInfo.back().DataType = typeid(T).hash_code();

                    #if SO_LOG_SetArgs
                        std::cout << "Set args index: "<< 
                            lastData.ArgumentsDataInfo.size() - 1 << std::endl;
                        
                        std::cout << "arg pointer: "<<&arg<<std::endl;
                        std::cout << "typeid(arg).name(): " << typeid(arg).name() <<std::endl;
                        std::cout <<    "typeid(arg).hash_code(): " << 
                                        typeid(arg).hash_code() << 
                                        std::endl;
                        
                        std::cout <<    "Set args value: "<< 
                                        (*static_cast<T*>(lastData.ArgumentsDataInfo.back().Data)) << 
                                        std::endl << 
                                        std::endl;
                        
                        std::cout << "Original Data: "<<std::endl;
                        PRINT_BYTES(arg);
                        
                        std::cout << "Copied Data: "<<std::endl;
                        PRINT_BYTES((*static_cast<T*>(lastData.ArgumentsDataInfo.back().Data)));
                        std::cout << std::endl;
                    #endif
                }
                
                return SetArgs(proxy, args...);
            }
            
            template<typename T, typename... Args>
            inline ArgumentsProxy& SetArgs( ArgumentsProxy& proxy,
                                            NonComparable<T> arg, Args... args)
            {
                return SetArgs(proxy, *arg.ReferenceVar, args...);
            }
            
            template<typename T, typename... Args>
            inline ArgumentsProxy& SetArgs( ArgumentsProxy& proxy,
                                            NonCopyable<T> arg, Args... args)
            {
                static_assert(  SO_ASSERT_FALSE<T>::value, 
                                "You can't pass non copyable argument to be set");
                
                return SetArgs(proxy, args...);
            }
            
            template<typename T, typename... Args>
            inline ArgumentsProxy& SetArgs( ArgumentsProxy& proxy,
                                            NonComparableCopyable<T> arg, Args... args)
            {
                static_assert(  SO_ASSERT_FALSE<T>::value, 
                                "You can't pass non copyable argument to be set");
                
                return SetArgs(proxy, args...);
            }
        
        public:
            inline Internal_ArgumentDataSetter(ArgumentInfosType& overrideArgumentsInfos) : 
                OverrideArgumentsInfos(overrideArgumentsInfos)
            {}
    };
}


#endif