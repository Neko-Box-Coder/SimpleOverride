#ifndef SIMPLE_OVERRIDE_FUNCTION_OVERRIDES_HPP
#define SIMPLE_OVERRIDE_FUNCTION_OVERRIDES_HPP

#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <typeinfo>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <set>

namespace SimpleOverride
{
    class FunctionOverrides;

    //==============================================================================
    //Public data structures
    //==============================================================================
    enum class ProxyType
    {
        COMMON,
        RETURN,
        ARGS
    };
    
    struct ArgInfo
    {
        void* ArgData = nullptr;
        std::function<void*(void*)> CopyConstructor;
        std::function<void(void*)> Destructor;
        size_t ArgSize = 0;
        size_t ArgTypeHash = 0;
        bool ArgSet = false;
    };
    
    struct Any
    {
        inline bool operator== (Any& other)
        {
            return true;
        }
        
        inline bool operator!= (Any& other)
        {
            return false;
        }
    
        friend std::ostream& operator<<(std::ostream& os, const Any& other)
        {
            os << "Any";
            return os;
        }
    } const ANY;
    
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
    
    //==============================================================================
    //Method Chaining Classes
    //==============================================================================
    //Common proxy class for method chaining
    template<typename DeriveType>
    class FunctionOverridesCommonProxy
    {
        friend class FunctionOverrides;

        protected:
            std::string FunctionSignatureName;
            FunctionOverrides& FunctionOverridesObj;
            const ProxyType FunctionProxyType;

        public:
            FunctionOverridesCommonProxy(std::string functionSignatureName, FunctionOverrides& functionOverridesObj, ProxyType functionProxyType) :  
                FunctionSignatureName(functionSignatureName),
                FunctionOverridesObj(functionOverridesObj),
                FunctionProxyType(functionProxyType)
            {}

            DeriveType& Times(int times);
            
            template<typename... Args>
            DeriveType& WhenCalledWith(Args... args);
            
            DeriveType& If(std::function<bool(std::vector<void*>& args)> condition);

            DeriveType& Otherwise_Do(std::function<void(std::vector<void*>& args)> action);

            DeriveType& WhenCalledExpectedly_Do(std::function<void(std::vector<void*>& args)> action);
    };
    
    //Override return proxy class for method chaining
    class FunctionOverridesReturnProxy : public FunctionOverridesCommonProxy<FunctionOverridesReturnProxy>
    {
        public:
            FunctionOverridesReturnProxy(std::string functionSignatureName, FunctionOverrides& functionOverridesObj, ProxyType functionProxyType) : 
                FunctionOverridesCommonProxy(functionSignatureName, functionOverridesObj, functionProxyType) 
            {}
            
            template<typename T>
            FunctionOverridesReturnProxy& ReturnsByAction(std::function<void(std::vector<void*>& args, void* out)> returnAction);
            
            template<typename T>
            FunctionOverridesReturnProxy& Returns(T returnData);
    };
    
    //Override arguments proxy class for method chaining
    class FunctionOverridesArgumentsProxy : public FunctionOverridesCommonProxy<FunctionOverridesArgumentsProxy>
    {
        public:
            FunctionOverridesArgumentsProxy(std::string functionSignatureName, FunctionOverrides& functionOverridesObj, ProxyType functionProxyType) : 
                FunctionOverridesCommonProxy(functionSignatureName, functionOverridesObj, functionProxyType) 
            {}
            
            template<typename T>
            FunctionOverridesArgumentsProxy& SetArgsByAction(std::function<void(std::vector<void*>& args, void* out)> setArgsAction);
            
            template<typename... Args>
            FunctionOverridesArgumentsProxy& SetArgs(Args... args);
    };

    //==============================================================================
    //Main Class
    //==============================================================================
    class FunctionOverrides
    {
        friend class FunctionOverridesCommonProxy<FunctionOverridesReturnProxy>;
        friend class FunctionOverridesCommonProxy<FunctionOverridesArgumentsProxy>;
        friend class FunctionOverridesReturnProxy;
        friend class FunctionOverridesArgumentsProxy;

        //==============================================================================
        //Data Structures for storing requirements and datas to be returned or set
        //==============================================================================
        private:
            struct DataInfo
            {
                std::size_t DataType = 0;
                void* Data = nullptr;
                std::function<void*(void*)> CopyConstructor;
                std::function<void(void*)> Destructor;
                std::function<void(std::vector<void*>& args, void* out)> DataAction;
                bool DataSet = false;
                bool DataActionSet = false;
            };

            struct ConditionInfo
            {
                std::function<bool(std::vector<void*>& args)> DataCondition;
                std::vector<ArgInfo> ArgsCondition = {};
                int Times = -1;
                int CalledTimes = 0;
                bool DataConditionSet = false;
            };

            struct ActionInfo
            {
                std::function<void(std::vector<void*>& args)> OtherwiseAction;
                std::function<void(std::vector<void*>&)> CorrectAction;
                bool OtherwiseActionSet = false;
                bool CorrectActionSet = false;
            };
            
            struct ReturnData
            {
                ConditionInfo ReturnConditionInfo;
                DataInfo ReturnDataInfo;
                ActionInfo ReturnActionInfo;
            };

            struct OverrideReturnDataInfo
            {
                std::vector<ReturnData> ReturnDatas = {};
            };
            
            struct ArgumentsData
            {
                ConditionInfo ArgumentsConditionInfo;
                std::vector<DataInfo> ArgumentsDataInfo;
                ActionInfo ArgumentsActionInfo;
            };

            struct OverrideArgumentInfo
            {
                std::vector<ArgumentsData> ArgumentsDatas = {};
            };

            std::unordered_map<std::string, OverrideReturnDataInfo> OverrideReturnInfos;
            std::unordered_map<std::string, OverrideArgumentInfo> OverrideArgumentsInfos;
        
        //==============================================================================
        //Methods for storing requirements and datas to be returned or set
        //==============================================================================
        private:
        
            #define INTERNAL_FO_UNCONST(targetType) typename std::remove_const<targetType>::type
            #define INTERNAL_FO_PURE_T INTERNAL_FO_UNCONST(T)
        
            //------------------------------------------------------------------------------
            //Methods for setting return data
            //------------------------------------------------------------------------------
            template<typename T>
            inline FunctionOverridesReturnProxy& ReturnsByAction(FunctionOverridesReturnProxy& proxy, 
                                                                std::function<void(std::vector<void*>& args, void* out)> returnAction)
            {
                ReturnData& lastData = OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back();

                lastData.ReturnDataInfo.DataAction = returnAction;
                lastData.ReturnDataInfo.DataActionSet = true;
                lastData.ReturnDataInfo.DataType = typeid(T).hash_code();
                return proxy;
            }
        
            template<typename T>
            inline FunctionOverridesReturnProxy& Returns(FunctionOverridesReturnProxy& proxy, T returnData)
            {
                ReturnData& lastData = OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back();
                lastData.ReturnDataInfo.Data = new T(returnData);
                lastData.ReturnDataInfo.CopyConstructor = [](void* data) { return new T(*static_cast<T*>(data)); };
                lastData.ReturnDataInfo.Destructor = [](void* data) { delete static_cast<T*>(data); }; 
                lastData.ReturnDataInfo.DataSet = true;
                lastData.ReturnDataInfo.DataType = typeid(T).hash_code();
                return proxy;
            }
            
            //------------------------------------------------------------------------------
            //Methods for setting arguments data
            //------------------------------------------------------------------------------
            template<typename T>
            inline FunctionOverridesArgumentsProxy& SetArgsByAction( FunctionOverridesArgumentsProxy& proxy,
                                                                    std::function<void(std::vector<void*>& args, void* out)> setArgsAction)
            {
                ArgumentsData& lastData = OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back();
                
                lastData.ArgumentsDataInfo.push_back(DataInfo());
                
                lastData.ArgumentsDataInfo.back().DataAction = setArgsAction;
                lastData.ArgumentsDataInfo.back().DataActionSet = true;
                lastData.ArgumentsDataInfo.back().DataType = typeid(T).hash_code();
                return proxy;
            }
            
            inline FunctionOverridesArgumentsProxy& SetArgs(FunctionOverridesArgumentsProxy& proxy)
            {
                return proxy;
            }
            
            #define FO_LOG_SetArgs 0
            
            #if 0 | FO_LOG_SetArgs
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
            inline FunctionOverridesArgumentsProxy& SetArgs( FunctionOverridesArgumentsProxy& proxy,
                                                            T arg, Args... args)
            {
                ArgumentsData& lastData = OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back();
                lastData.ArgumentsDataInfo.push_back(DataInfo());
                
                if(!std::is_same<T, Any>())
                {
                    lastData.ArgumentsDataInfo.back().Data = new T(arg);
                    lastData.ArgumentsDataInfo.back().CopyConstructor = [](void* data) { return new T(*static_cast<T*>(data)); };
                    lastData.ArgumentsDataInfo.back().Destructor = [](void* data) { delete static_cast<T*>(data); };
                    lastData.ArgumentsDataInfo.back().DataSet = true;
                    lastData.ArgumentsDataInfo.back().DataType = typeid(T).hash_code();

                    #if FO_LOG_SetArgs
                        std::cout << "Set args index: "<<lastData.ArgumentsDataInfo.size() - 1 << std::endl;
                        std::cout << "arg pointer: "<<&arg<<std::endl;
                        std::cout << "typeid(arg).name(): " << typeid(arg).name() <<std::endl;
                        std::cout << "typeid(arg).hash_code(): " << typeid(arg).hash_code() <<std::endl;
                        std::cout << "Set args value: "<< (*static_cast<T*>(lastData.ArgumentsDataInfo.back().Data)) << std::endl << std::endl;
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
            inline FunctionOverridesArgumentsProxy& SetArgs( FunctionOverridesArgumentsProxy& proxy,
                                                            NonComparable<T> arg, Args... args)
            {
                ArgumentsData& lastData = OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back();
                lastData.ArgumentsDataInfo.push_back(DataInfo());
                
                if(!std::is_same<T, Any>())
                {
                    lastData.ArgumentsDataInfo.back().Data = new T(*arg.ReferenceVar);
                    lastData.ArgumentsDataInfo.back().CopyConstructor = [](void* data) { return new T(*static_cast<T*>(data)); };
                    lastData.ArgumentsDataInfo.back().Destructor = [](void* data) { delete static_cast<T*>(data); };
                    lastData.ArgumentsDataInfo.back().DataSet = true;
                    lastData.ArgumentsDataInfo.back().DataType = typeid(T).hash_code();

                    #if FO_LOG_SetArgs
                        std::cout << "Set args index: "<<lastData.ArgumentsDataInfo.size() - 1 << std::endl;
                        std::cout << "arg pointer: "<<&arg<<std::endl;
                        std::cout << "typeid(arg).name(): " << typeid(arg).name() <<std::endl;
                        std::cout << "typeid(arg).hash_code(): " << typeid(arg).hash_code() <<std::endl;
                        std::cout << "Set args value: "<< (*static_cast<T*>(lastData.ArgumentsDataInfo.back().Data)) << std::endl << std::endl;
                        std::cout << "Original Data: "<<std::endl;
                        PRINT_BYTES(arg);
                        
                        std::cout << "Copied Data: "<<std::endl;
                        PRINT_BYTES((*static_cast<T*>(lastData.ArgumentsDataInfo.back().Data)));
                        std::cout << std::endl;
                    #endif
                }
                
                return SetArgs(proxy, args...);
            }
            
            template<typename T>
            struct FO_ASSERT_FALSE : std::false_type { };
            
            template<typename T, typename... Args>
            inline FunctionOverridesArgumentsProxy& SetArgs( FunctionOverridesArgumentsProxy& proxy,
                                                            NonCopyable<T> arg, Args... args)
            {
                static_assert(FO_ASSERT_FALSE<T>::value, "You can't pass non copyable argument to be set");
                return SetArgs(proxy, args...);
            }
            
            template<typename T, typename... Args>
            inline FunctionOverridesArgumentsProxy& SetArgs( FunctionOverridesArgumentsProxy& proxy,
                                                            NonComparableCopyable<T> arg, Args... args)
            {
                static_assert(FO_ASSERT_FALSE<T>::value, "You can't pass non copyable argument to be set");
                return SetArgs(proxy, args...);
            }
            
            
            //------------------------------------------------------------------------------
            //Methods for setting requirements
            //------------------------------------------------------------------------------
            template<typename DeriveType>
            inline DeriveType& Times(FunctionOverridesCommonProxy<DeriveType>& proxy, int times)
            {
                switch(proxy.FunctionProxyType)
                {
                    case ProxyType::RETURN:
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnConditionInfo.Times = times;
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsConditionInfo.Times = times;
                        break;
                    case ProxyType::COMMON:
                        std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                        assert(false);
                        exit(1);
                        break;
                }
                return *static_cast<DeriveType*>(&proxy);
            }
            
            template<typename DeriveType>
            inline DeriveType& WhenCalledWith(FunctionOverridesCommonProxy<DeriveType>& proxy)
            {
                return *static_cast<DeriveType*>(&proxy);
            }

            template<typename DeriveType, typename T, typename... Args>
            inline DeriveType& WhenCalledWith(FunctionOverridesCommonProxy<DeriveType>& proxy, NonComparable<T> arg, Args... args)
            {
                static_assert(FO_ASSERT_FALSE<T>::value, "You can't check non comparable variable");
                return *static_cast<DeriveType*>(&proxy);
            }
            
            template<typename DeriveType, typename T, typename... Args>
            inline DeriveType& WhenCalledWith(FunctionOverridesCommonProxy<DeriveType>& proxy, NonComparableCopyable<T> arg, Args... args)
            {
                static_assert(FO_ASSERT_FALSE<T>::value, "You can't check non comparable variable");
                return *static_cast<DeriveType*>(&proxy);
            }

            template<typename DeriveType, typename T, typename... Args>
            inline DeriveType& WhenCalledWith(FunctionOverridesCommonProxy<DeriveType>& proxy, T arg, Args... args)
            {
                ArgInfo curArg;
                if(!std::is_same<T, Any>())
                {
                    curArg.ArgData = new T(arg);
                    curArg.CopyConstructor = [](void* data) { return new T(*static_cast<T*>(data)); };
                    curArg.Destructor = [](void* data){ delete static_cast<T*>(data); };
                    curArg.ArgSize = sizeof(T);
                    curArg.ArgTypeHash = typeid(T).hash_code();
                    curArg.ArgSet = true;
                    
                    #if 0
                        std::cout << "typeid(T).name(): "<<typeid(T).name() <<"\n";
                        std::cout << "sizeof(T): " << sizeof(T) << "\n";
                        std::cout << "typeid(T).hash_code(): " << typeid(T).hash_code() << "\n";
                    #endif
                }

                switch(proxy.FunctionProxyType)
                {
                    case ProxyType::RETURN:
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back()
                            .ReturnConditionInfo.ArgsCondition.push_back(curArg);
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back()
                            .ArgumentsConditionInfo.ArgsCondition.push_back(curArg);
                        break;
                    case ProxyType::COMMON:
                        std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                        assert(false);
                        exit(1);
                        break;
                }

                return WhenCalledWith(proxy, args...);
            }
            
            template<typename DeriveType, typename T, typename... Args>
            inline DeriveType& WhenCalledWith(FunctionOverridesCommonProxy<DeriveType>& proxy, NonCopyable<T> arg, Args... args)
            {
                ArgInfo curArg;
                curArg.ArgData = const_cast<INTERNAL_FO_PURE_T*>(arg.ReferenceVar);
                curArg.CopyConstructor = [](void* data) { return data; };
                curArg.Destructor = [](void* data){ };
                curArg.ArgSize = sizeof(T);
                curArg.ArgTypeHash = typeid(T).hash_code();
                curArg.ArgSet = true;
                
                #if 0
                    std::cout << "typeid(T).name(): "<<typeid(T).name() <<"\n";
                    std::cout << "sizeof(T): " << sizeof(T) << "\n";
                    std::cout << "typeid(T).hash_code(): " << typeid(T).hash_code() << "\n";
                #endif

                switch(proxy.FunctionProxyType)
                {
                    case ProxyType::RETURN:
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back()
                            .ReturnConditionInfo.ArgsCondition.push_back(curArg);
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back()
                            .ArgumentsConditionInfo.ArgsCondition.push_back(curArg);
                        break;
                    case ProxyType::COMMON:
                        std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                        assert(false);
                        exit(1);
                        break;
                }

                return WhenCalledWith(proxy, args...);
            }
            
            template<typename DeriveType>
            inline DeriveType& If(   FunctionOverridesCommonProxy<DeriveType>& proxy, 
                                    std::function<bool(std::vector<void*>& args)> condition)
            {
                switch(proxy.FunctionProxyType)
                {
                    case ProxyType::RETURN:
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnConditionInfo.DataCondition = condition;
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnConditionInfo.DataConditionSet = true;
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsConditionInfo.DataCondition = condition;
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsConditionInfo.DataConditionSet = true;
                        break;
                    case ProxyType::COMMON:
                        std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                        assert(false);
                        exit(1);
                        break;
                }
            
                return *static_cast<DeriveType*>(&proxy);
            }
            
            template<typename DeriveType>
            inline DeriveType& Otherwise_Do( FunctionOverridesCommonProxy<DeriveType>& proxy, 
                                            std::function<void(std::vector<void*>& args)> action)
            {
                switch(proxy.FunctionProxyType)
                {
                    case ProxyType::RETURN:
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnActionInfo.OtherwiseAction = action;
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnActionInfo.OtherwiseActionSet = true;
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsActionInfo.OtherwiseAction = action;
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsActionInfo.OtherwiseActionSet = true;
                        break;
                    case ProxyType::COMMON:
                        std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                        assert(false);
                        exit(1);
                        break;
                }
                
                return *static_cast<DeriveType*>(&proxy);
            }
            
            template<typename DeriveType>
            inline DeriveType& WhenCalledExpectedly_Do(  FunctionOverridesCommonProxy<DeriveType>& proxy, 
                                                        std::function<void(std::vector<void*>& args)> action)
            {
                switch(proxy.FunctionProxyType)
                {
                    case ProxyType::RETURN:
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnActionInfo.CorrectAction = action;
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnActionInfo.CorrectActionSet = true;
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsActionInfo.CorrectAction = action;
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsActionInfo.CorrectActionSet = true;
                        break;
                    case ProxyType::COMMON:
                        std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                        assert(false);
                        exit(1);
                        break;
                }
    
                return *static_cast<DeriveType*>(&proxy);
            }

        //==============================================================================
        //Implementation methods for querying (and setting) the correct return or arugment data
        //==============================================================================
        private:
            //Appending arguments from function calls
            inline void AppendArguments(std::vector<void*>& argumentsList){};

            template<typename T, typename... Args>
            inline void AppendArguments(std::vector<void*>& argumentsList, T& arg, Args&... args)
            {
                argumentsList.push_back((INTERNAL_FO_PURE_T*)&arg);
                AppendArguments(argumentsList, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendArguments(std::vector<void*>& argumentsList, NonCopyable<T>& arg, Args&... args)
            {
                argumentsList.push_back((INTERNAL_FO_PURE_T*)&arg);
                AppendArguments(argumentsList, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendArguments(std::vector<void*>& argumentsList, NonComparable<T>& arg, Args&... args)
            {
                argumentsList.push_back((INTERNAL_FO_PURE_T*)&arg);
                AppendArguments(argumentsList, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendArguments(std::vector<void*>& argumentsList, NonComparableCopyable<T>& arg, Args&... args)
            {
                argumentsList.push_back((INTERNAL_FO_PURE_T*)&arg);
                AppendArguments(argumentsList, args...);
            }
            
            inline void AppendDereferenceArgsInfo(std::vector<ArgInfo>& argumentsList) {}

            template<typename T, typename... Args>
            inline void AppendDereferenceArgsInfo(std::vector<ArgInfo>& argumentsList, T& arg, Args&... args)
            {
                ArgInfo curArgInfo;
                if(!std::is_same<T, Any>())
                {
                    curArgInfo.ArgSize = sizeof(INTERNAL_FO_PURE_T);
                    curArgInfo.ArgTypeHash = typeid(INTERNAL_FO_PURE_T).hash_code();
                    curArgInfo.ArgSet = true;
                }

                argumentsList.push_back(curArgInfo);
                AppendDereferenceArgsInfo(argumentsList, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendDereferenceArgsInfo(std::vector<ArgInfo>& argumentsList, NonCopyable<T>& arg, Args&... args)
            {
                AppendDereferenceArgsInfo(argumentsList, (INTERNAL_FO_PURE_T&)arg, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendDereferenceArgsInfo(std::vector<ArgInfo>& argumentsList, NonComparable<T>& arg, Args&... args)
            {
                AppendDereferenceArgsInfo(argumentsList, (INTERNAL_FO_PURE_T&)arg, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendDereferenceArgsInfo(std::vector<ArgInfo>& argumentsList, NonComparableCopyable<T>& arg, Args&... args)
            {
                AppendDereferenceArgsInfo(argumentsList, (INTERNAL_FO_PURE_T&)arg, args...);
            }
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline void AppendDereferenceArgsInfo(std::vector<ArgInfo>& argumentsList, T* arg, Args&... args)
            {
                AppendDereferenceArgsInfo(argumentsList, *arg, args...);
            }

            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex){ return true; };

            #define FO_LOG_CheckArguments 0

            template<typename T, typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, T& arg, Args&... args)
            {
                #if FO_LOG_CheckArguments
                    std::cout <<"CheckArguments index: "<<argIndex<<"\n";
                #endif
            
                if(argIndex >= argumentsListToCheck.size())
                    return false;

                if(argumentsListToCheck[argIndex].ArgSet)
                {
                    if(sizeof(INTERNAL_FO_PURE_T) != argumentsListToCheck[argIndex].ArgSize)
                    {
                        #if FO_LOG_CheckArguments
                            std::cout <<"sizeof(INTERNAL_FO_PURE_T): "<<sizeof(INTERNAL_FO_PURE_T)<<"\n";
                            std::cout <<"sizeof(T): "<<sizeof(T)<<"\n";
                            std::cout <<"argumentsListToCheck["<<argIndex<<"].ArgSize: "<<argumentsListToCheck[argIndex].ArgSize<<"\n";
                        #endif
                        return false;
                    }

                    if(typeid(INTERNAL_FO_PURE_T).hash_code() != argumentsListToCheck[argIndex].ArgTypeHash)
                    {
                        #if FO_LOG_CheckArguments
                            std::cout <<"typeid(INTERNAL_FO_PURE_T).hash_code(): "<<typeid(INTERNAL_FO_PURE_T).hash_code()<<"\n";
                            std::cout <<"argumentsListToCheck["<<argIndex<<"].ArgTypeHash: "<<argumentsListToCheck[argIndex].ArgTypeHash<<"\n";
                        #endif
                        return false;
                    }

                    if(arg != *reinterpret_cast<INTERNAL_FO_PURE_T*>(argumentsListToCheck[argIndex].ArgData))
                    {
                        #if FO_LOG_CheckArguments
                            std::cout <<"arg != *reinterpret_cast<INTERNAL_FO_PURE_T*>(argumentsListToCheck[argIndex].ArgData\n";
                        #endif
                        return false;
                    }
                }            
                
                #if FO_LOG_CheckArguments
                    std::cout <<"CheckArguments index: "<<argIndex<<" passed\n";
                #endif
                
                return CheckArguments(argumentsListToCheck, ++argIndex, args...);
            }
            
            template<typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, Any& arg, Args&... args)
            {
                #if FO_LOG_CheckArguments
                    std::cout <<"CheckArguments index: "<<argIndex<<"\n";
                #endif
            
                if(argIndex >= argumentsListToCheck.size())
                    return false;

                if(argumentsListToCheck[argIndex].ArgSet)
                    return false;
                
                #if FO_LOG_CheckArguments
                    std::cout <<"CheckArguments index: "<<argIndex<<" passed\n";
                #endif
                
                return CheckArguments(argumentsListToCheck, ++argIndex, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, NonCopyable<T>& arg, Args&... args)
            {
                return CheckArguments(argumentsListToCheck, argIndex, (T&)arg, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, NonComparable<T>& arg, Args&... args)
            {
                if(argIndex >= argumentsListToCheck.size())
                    return false;
            
                return !argumentsListToCheck[argIndex].ArgSet ? CheckArguments(argumentsListToCheck, ++argIndex, args...) : false;
            }
            
            template<typename T, typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, NonComparableCopyable<T>& arg, Args&... args)
            {
                if(argIndex >= argumentsListToCheck.size())
                    return false;

                return !argumentsListToCheck[argIndex].ArgSet ? CheckArguments(argumentsListToCheck, ++argIndex, args...) : false;
            }
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, T*& arg, Args&... args)
            {
                return CheckArguments(argumentsListToCheck, argIndex, *arg, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, const T& arg, Args&... args)
            {
                return CheckArguments(argumentsListToCheck, argIndex, const_cast<INTERNAL_FO_PURE_T&>(arg), args...);
            }
            
            #define FO_LOG_GetCorrectReturnDataInfo 0

            template<typename T, typename... Args>
            inline int GetCorrectReturnDataInfo(T& returnRef, std::string functionName, Args&... args)
            {
                if(OverrideReturnInfos.find(functionName) == OverrideReturnInfos.end())
                {
                    std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                    assert(false);
                    exit(1);
                }
                
                #if FO_LOG_GetCorrectReturnDataInfo
                    std::cout <<"GetCorrectReturnDataInfo called\n";
                #endif

                std::vector<void*> argumentsList;
                AppendArguments(argumentsList, args...);
                
                std::vector<ReturnData>& curReturnDatas = OverrideReturnInfos[functionName].ReturnDatas;
                int returnIndex = -1;
                for(int i = 0; i < curReturnDatas.size(); i++)
                {
                    #if FO_LOG_GetCorrectReturnDataInfo
                        std::cout << "Checking return data["<<i<<"]\n";
                    #endif

                    //Check return data exist
                    if(!curReturnDatas[i].ReturnDataInfo.DataSet && !curReturnDatas[i].ReturnDataInfo.DataActionSet)
                    {
                        #if FO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at return data exist\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);
                        continue;
                    }
                
                    //Check return type
                    if(curReturnDatas[i].ReturnDataInfo.DataType != typeid(INTERNAL_FO_PURE_T).hash_code())
                    {
                        #if FO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at return type\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);
                        
                        continue;
                    }

                    //Check condition
                    if(curReturnDatas[i].ReturnConditionInfo.DataConditionSet && !curReturnDatas[i].ReturnConditionInfo.DataCondition(argumentsList))
                    {
                        #if FO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at Check condition\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);
                        
                        continue;
                    }

                    //Check parameter
                    if( !curReturnDatas[i].ReturnConditionInfo.ArgsCondition.empty() && 
                        !CheckArguments(curReturnDatas[i].ReturnConditionInfo.ArgsCondition, 0, args...))
                    {
                        #if FO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at Check parameter\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);

                        continue;
                    }
                        
                    //Check times
                    if( curReturnDatas[i].ReturnConditionInfo.Times >= 0 && 
                        curReturnDatas[i].ReturnConditionInfo.CalledTimes >= curReturnDatas[i].ReturnConditionInfo.Times)
                    {
                        #if FO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at Check times\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);

                        continue;
                    }
                    
                    #if FO_LOG_GetCorrectReturnDataInfo
                        std::cout << "Return data found: "<<i<<"\n";
                    #endif
                    returnIndex = i;
                    break;
                }
                
                //Deallocating argumentsList
                //for(int i = 0; i < argumentsList.size(); i++)
                //    argumentsList[i].Destructor(argumentsList[i].ArgData);
                
                return returnIndex;
            }
            
            #define FO_LOG_GetCorrectArgumentsDataInfo 0
            
            template<typename... Args>
            inline int GetCorrectArgumentsDataInfo(std::string functionName, Args&... args)
            {
                if(OverrideArgumentsInfos.find(functionName) == OverrideArgumentsInfos.end())
                {
                    std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                    assert(false);
                    exit(1);
                }
                
                #if FO_LOG_GetCorrectArgumentsDataInfo
                    std::cout <<"GetCorrectArgumentsDataInfo called\n";
                #endif
                
                std::vector<void*> argumentsList;
                AppendArguments(argumentsList, args...);
                
                std::vector<ArgInfo> deRefArgumentsList;
                AppendDereferenceArgsInfo(deRefArgumentsList, args...);
                
                std::vector<ArgumentsData>& curArgData = OverrideArgumentsInfos[functionName].ArgumentsDatas;
                int returnIndex = -1;
                for(int i = 0; i < curArgData.size(); i++)
                {
                    #if FO_LOG_GetCorrectArgumentsDataInfo
                        std::cout << "Checking arg data["<<i<<"]\n";
                    #endif
                
                    //Check set argument data exist
                    if(curArgData[i].ArgumentsDataInfo.size() != deRefArgumentsList.size())
                    {
                        #if FO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check set argument data exist\n";
                        #endif
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.OtherwiseAction(argumentsList);
                        
                        continue;
                    }
                
                    //Check arguments types
                    bool argumentTypeFailed = false;
                    for(int j = 0; j < curArgData[i].ArgumentsDataInfo.size(); j++)
                    {
                        if( curArgData[i].ArgumentsDataInfo[j].DataSet &&
                            curArgData[i].ArgumentsDataInfo[j].DataType != deRefArgumentsList[j].ArgTypeHash)
                        {
                            #if FO_LOG_GetCorrectArgumentsDataInfo
                                std::cout << "Failed at Check arguments types\n";
                            #endif
                            argumentTypeFailed = true;
                            break;;
                        }
                    }
                    if(argumentTypeFailed)
                    {
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.OtherwiseAction(argumentsList);

                        continue;
                    }

                    //Check condition
                    if(curArgData[i].ArgumentsConditionInfo.DataConditionSet && !curArgData[i].ArgumentsConditionInfo.DataCondition(argumentsList))
                    {
                        #if FO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check condition\n";
                        #endif
                        continue;
                    }

                    //Check parameter
                    if( !curArgData[i].ArgumentsConditionInfo.ArgsCondition.empty() && 
                        !CheckArguments(curArgData[i].ArgumentsConditionInfo.ArgsCondition, 0, args...))
                    {
                        #if FO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check parameter\n";
                        #endif
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.OtherwiseAction(argumentsList);
                        continue;
                    }
                        
                    //Check times
                    if( curArgData[i].ArgumentsConditionInfo.Times >= 0 && 
                        curArgData[i].ArgumentsConditionInfo.CalledTimes >= curArgData[i].ArgumentsConditionInfo.Times)
                    {
                        #if FO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check times\n";
                        #endif
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.OtherwiseAction(argumentsList);
                        continue;
                    }
                    
                    #if FO_LOG_GetCorrectArgumentsDataInfo
                        std::cout << "Argument data found: "<<i<<"\n";
                    #endif
                    
                    returnIndex = i;
                    break;
                }
                
                //Deallocating argumentsList
                //for(int i = 0; i < argumentsList.size(); i++)
                //    argumentsList[i].Destructor(argumentsList[i].ArgData);
                
                //Deallocating derefArgumentsList
                //for(int i = 0; i < derefArgumentsList.size(); i++)
                //    derefArgumentsList[i].Destructor(derefArgumentsList[i].ArgData);

                return returnIndex;
            }
            
            #define FO_LOG_ModifyArgs 0
            
            inline void ModifyArgs(std::vector<void*>& argumentsList, std::vector<DataInfo>& argsData, int index) {}

            template<typename T, typename... Args>
            inline void ModifyArgs(std::vector<void*>& argumentsList, std::vector<DataInfo>& argsData, int index, T& arg, Args&... args)
            {
                if(!argsData[index].DataSet && !argsData[index].DataActionSet)
                {
                    ModifyArgs(argumentsList, argsData, ++index, args...);
                    return;
                }
            
                if(argsData[index].DataSet)
                {
                    INTERNAL_FO_PURE_T& pureArg = const_cast<INTERNAL_FO_PURE_T&>(arg); 
                    pureArg = *static_cast<INTERNAL_FO_PURE_T*>(argsData[index].Data);
                    #if FO_LOG_ModifyArgs
                        std::cout << "modified index: "<<index << std::endl;
                        std::cout << "typeid(arg).name(): " << typeid(arg).name() <<std::endl;
                        std::cout << "typeid(arg).hash_code(): " << typeid(arg).hash_code() <<std::endl;
                        std::cout << "argsData[index].DataType: " << argsData[index].DataType <<std::endl;
                        std::cout << "arg value: "<< arg << std::endl;
                        std::cout << "modified value: "<< (*static_cast<T*>(argsData[index].Data)) << std::endl << std::endl;
                        std::cout << "modified value bytes:" << std::endl;
                        
                        PRINT_BYTES((*static_cast<T*>(argsData[index].Data)));
                        std::cout << std::endl;
                    #endif
                }
                else
                    argsData[index].DataAction(argumentsList, &const_cast<INTERNAL_FO_PURE_T&>(arg));

                ModifyArgs(argumentsList, argsData, ++index, args...);
            }
            
            template<typename T, typename... Args>
            inline void ModifyArgs(std::vector<void*>& argumentsList, std::vector<DataInfo>& argsData, int index, NonComparable<T>& arg, Args&... args)
            {
                if(!argsData[index].DataSet && !argsData[index].DataActionSet)
                {
                    ModifyArgs(argumentsList, argsData, ++index, args...);
                    return;
                }
            
                #if FO_LOG_ModifyArgs
                    std::cout << "modified index: "<<index << std::endl;
                    std::cout << "typeid(arg).name(): " << typeid(arg).name() <<std::endl;
                    std::cout << "typeid(arg).hash_code(): " << typeid(arg).hash_code() <<std::endl;
                    std::cout << "arg value: "<< arg << std::endl;
                    std::cout << std::endl;
                #endif

                if(argsData[index].DataSet)
                {
                    INTERNAL_FO_PURE_T& pureArg = (INTERNAL_FO_PURE_T&)(arg); 
                    pureArg = *static_cast<INTERNAL_FO_PURE_T*>(argsData[index].Data);
                    #if FO_LOG_ModifyArgs
                        std::cout << "argsData[index].DataType: " << argsData[index].DataType <<std::endl;
                        std::cout << "modified value: "<< (*static_cast<T*>(argsData[index].Data)) << std::endl << std::endl;
                        std::cout << "modified value bytes:" << std::endl;
                        
                        PRINT_BYTES((*static_cast<T*>(argsData[index].Data)));
                        std::cout << std::endl;
                    #endif
                }
                else
                    argsData[index].DataAction(argumentsList, &((INTERNAL_FO_PURE_T&)(arg)));

                ModifyArgs(argumentsList, argsData, ++index, args...);
            }
            
            template<typename T, typename... Args>
            inline void ModifyArgs(std::vector<void*>& argumentsList, std::vector<DataInfo>& argsData, int index, NonCopyable<T>& arg, Args&... args)
            {
                #if FO_LOG_ModifyArgs
                    std ::cout <<"Skipping ModifyArgs for index "<<index << " for NonCopyable\n";
                #endif
                ModifyArgs(argumentsList, argsData, ++index, args...);
            }
            
            template<typename T, typename... Args>
            inline void ModifyArgs(std::vector<void*>& argumentsList, std::vector<DataInfo>& argsData, int index, NonComparableCopyable<T>& arg, Args&... args)
            {
                #if FO_LOG_ModifyArgs
                    std ::cout <<"Skipping ModifyArgs for index "<<index << " for NonCopyable\n";
                #endif
                ModifyArgs(argumentsList, argsData, ++index, args...);
            }
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline void ModifyArgs(std::vector<void*>& argumentsList, std::vector<DataInfo>& argsData, int index, T*& arg, Args&... args)
            {
                ModifyArgs(argumentsList, argsData, index, *arg, args...);
            }
            
            template<typename T, typename... Args>
            inline void ModifyArgs(std::vector<void*>& argumentsList, std::vector<DataInfo>& argsData, int index, const T& arg, Args&... args)
            {
                if(!argsData[index].DataSet && !argsData[index].DataActionSet)
                {
                    ModifyArgs(argumentsList, argsData, ++index, args...);
                    return;
                }
                
                #if FO_LOG_ModifyArgs
                    std::cout << "modified index: "<<index << std::endl;
                    std::cout << "typeid(arg).name(): " << typeid(arg).name() <<std::endl;
                    std::cout << "typeid(arg).hash_code(): " << typeid(arg).hash_code() <<std::endl;
                    std::cout << "arg value: "<< arg << std::endl;
                    std::cout << std::endl;
                #endif
                
                if(argsData[index].DataSet)
                {
                    std::cout << "[ERROR] Data cannot be set for const arguments" << std::endl;
                    assert(false);
                    exit(1);
                }
                else
                {
                    std::cout << "[WARNING] DataAction is called on const argument, is this intentional?" << std::endl;
                    argsData[index].DataAction(argumentsList, &((INTERNAL_FO_PURE_T&)(arg)));
                }
                
                ModifyArgs(argumentsList, argsData, ++index, args...);
            }

            template<typename... Args>
            inline void ModifyArgs(std::vector<void*>& argumentsList, std::vector<DataInfo>& argsData, int index, const Any& arg, Args&... args)
            {
                #if FO_LOG_ModifyArgs
                    std ::cout <<"Skipping ModifyArgs for index "<<index << " for Any\n";
                #endif
                ModifyArgs(argumentsList, argsData, ++index, args...);
            }

        //==============================================================================
        //Public facing methods for overriding returns or arguments
        //==============================================================================
        public:
            inline FunctionOverrides(const FunctionOverrides& other)
            {
                *this = other;
            }
        
            inline FunctionOverrides& operator=(const FunctionOverrides& other)
            {
                if(this == &other)
                    return *this;
            
                OverrideReturnInfos = other.OverrideReturnInfos;
                OverrideArgumentsInfos = other.OverrideArgumentsInfos;
            
                for(auto it = OverrideReturnInfos.begin(); it != OverrideReturnInfos.end(); it++)
                {
                    for(int i = 0; i < it->second.ReturnDatas.size(); i++)
                    {
                        DataInfo& curReturnDataInfo = it->second.ReturnDatas[i].ReturnDataInfo;
                        if(curReturnDataInfo.DataSet)
                            curReturnDataInfo.Data = curReturnDataInfo.CopyConstructor(curReturnDataInfo.Data);
                        
                        for(int j = 0; j < it->second.ReturnDatas[i].ReturnConditionInfo.ArgsCondition.size(); j++)
                        {
                            ArgInfo& curArgDataInfo = it->second.ReturnDatas[i].ReturnConditionInfo.ArgsCondition[j];
                            
                            if(curArgDataInfo.ArgSet)
                                curArgDataInfo.ArgData = curArgDataInfo.CopyConstructor(curArgDataInfo.ArgData);
                        }
                    }
                }
                
                for(auto it = OverrideArgumentsInfos.begin(); it != OverrideArgumentsInfos.end(); it++)
                {
                    for(int i = 0; i < it->second.ArgumentsDatas.size(); i++)
                    {
                        for(int j = 0; j < it->second.ArgumentsDatas[i].ArgumentsDataInfo.size(); j++)
                        {
                            DataInfo& curArgDataInfo = it->second.ArgumentsDatas[i].ArgumentsDataInfo[j];
                            
                            if(curArgDataInfo.DataSet)
                                curArgDataInfo.Data = curArgDataInfo.CopyConstructor(curArgDataInfo.Data);
                        }
                        
                        for(int j = 0; j < it->second.ArgumentsDatas[i].ArgumentsConditionInfo.ArgsCondition.size(); j++)
                        {
                            ArgInfo& curArgDataInfo = it->second.ArgumentsDatas[i].ArgumentsConditionInfo.ArgsCondition[j];
                            
                            if(curArgDataInfo.ArgSet)
                                curArgDataInfo.ArgData = curArgDataInfo.CopyConstructor(curArgDataInfo.ArgData);
                        }
                    }
                }
                
                return *this;
            }
            
            FunctionOverrides() = default;
        
            inline ~FunctionOverrides()
            {
                for(auto it = OverrideReturnInfos.begin(); it != OverrideReturnInfos.end(); it++)
                {
                    for(int i = 0; i < it->second.ReturnDatas.size(); i++)
                    {
                        //Free return data
                        DataInfo& curDataInfo = it->second.ReturnDatas[i].ReturnDataInfo;
                        if(curDataInfo.DataSet)
                            curDataInfo.Destructor(curDataInfo.Data);
                        
                        //Free argument condition data
                        for(int j = 0; j < it->second.ReturnDatas[i].ReturnConditionInfo.ArgsCondition.size(); j++)
                        {
                            ArgInfo& curArgInfos = it->second.ReturnDatas[i].ReturnConditionInfo.ArgsCondition[j];
                            if(curArgInfos.ArgSet)
                                curArgInfos.Destructor(curArgInfos.ArgData);
                        }
                    }
                }
                
                for(auto it = OverrideArgumentsInfos.begin(); it != OverrideArgumentsInfos.end(); it++)
                {
                    for(int i = 0; i < it->second.ArgumentsDatas.size(); i++)
                    {
                        //Free set arguments data
                        for(int j = 0; j < it->second.ArgumentsDatas[i].ArgumentsDataInfo.size(); j++)
                        {
                            DataInfo& curDataInfo = it->second.ArgumentsDatas[i].ArgumentsDataInfo[j];
                            if(curDataInfo.DataSet)
                                curDataInfo.Destructor(curDataInfo.Data);
                        }
                        
                        //Free argument condition data
                        for(int j = 0; j < it->second.ArgumentsDatas[i].ArgumentsConditionInfo.ArgsCondition.size(); j++)
                        {
                            ArgInfo& curArgInfo = it->second.ArgumentsDatas[i].ArgumentsConditionInfo.ArgsCondition[j];
                            if(curArgInfo.ArgSet)
                                curArgInfo.Destructor(curArgInfo.ArgData);
                        }
                    }
                }
            }

            //------------------------------------------------------------------------------
            //Overrding Returns
            //------------------------------------------------------------------------------
            #define FO_LOG_OverrideReturns 0
            inline FunctionOverridesReturnProxy Internal_OverrideReturns(std::string functionName)
            {
                #if FO_LOG_OverrideReturns
                    std::cout << "OverrideReturns\n";
                    std::cout << "functionName: "<<functionName << "\n";
                #endif

                OverrideReturnInfos[functionName].ReturnDatas.push_back(ReturnData());
                return FunctionOverridesReturnProxy(functionName, *this, ProxyType::RETURN);
            }
            
            inline void Internal_ClearOverrideReturns(std::string functionName)
            {
                if(OverrideReturnInfos.find(functionName) != OverrideReturnInfos.end())
                    OverrideReturnInfos.erase(functionName);
            }
            
            inline void ClearAllOverrideReturns()
            {
                OverrideReturnInfos.clear();
            }
            
            #define FO_LOG_CheckOverrideAndReturn 0
            
            template<typename T, typename... Args>
            inline bool Internal_CheckOverrideAndReturn(T& returnRef, std::string functionName, Args&... args)
            {
                #if FO_LOG_CheckOverrideAndReturn
                    std::cout << "CheckOverrideAndReturn\n";
                    std::cout << "functionName: "<<functionName << "\n";
                #endif
                if(OverrideReturnInfos.find(functionName) == OverrideReturnInfos.end())
                    return false;
            
                int correctDataIndex = GetCorrectReturnDataInfo(returnRef, functionName, args...);
                
                std::vector<void*> argumentsList;
                AppendArguments(argumentsList, args...);
                
                //Called correctly action
                bool returnResult = false;
                if(correctDataIndex != -1)
                {
                    ReturnData& correctData = OverrideReturnInfos[functionName].ReturnDatas[correctDataIndex];
                    correctData.ReturnConditionInfo.CalledTimes++;
                    
                    if(correctData.ReturnActionInfo.CorrectActionSet)
                        correctData.ReturnActionInfo.CorrectAction(argumentsList);
                    
                    if(correctData.ReturnDataInfo.DataSet)
                        returnRef = *reinterpret_cast<T*>(correctData.ReturnDataInfo.Data);
                    else
                        correctData.ReturnDataInfo.DataAction(argumentsList, &returnRef);

                    assert(correctData.ReturnDataInfo.DataSet || correctData.ReturnDataInfo.DataActionSet);
                    returnResult = true;
                }
                
                //Deallocating argumentsList
                //for(int i = 0; i < argumentsList.size(); i++)
                //    argumentsList[i].Destructor(argumentsList[i].ArgData);

                return returnResult;
            }
            
            #define FO_RETURN_IF_FOUND(functionOverrideObj, functionSig, returnType, ...)\
            do\
            {\
                returnType returnVal;\
                if(functionOverrideObj.CheckOverrideAndReturn(returnVal, functionSig, __VA_ARGS__))\
                    return returnVal;\
            } while(0)


            //------------------------------------------------------------------------------
            //Overrding Arguments
            //------------------------------------------------------------------------------
            #define FO_LOG_OverrideArgs 0

            inline FunctionOverridesArgumentsProxy Internal_OverrideArgs(std::string functionName)
            {
                #if FO_LOG_OverrideArgs
                    std::cout << "OverrideArgs\n";
                    std::cout << "functionName: "<<functionName << "\n";
                #endif

                OverrideArgumentsInfos[functionName].ArgumentsDatas.push_back(ArgumentsData());
                return FunctionOverridesArgumentsProxy(functionName, *this, ProxyType::ARGS);
            }
            
            inline void Internal_ClearOverrideArgs(std::string functionName)
            {
                if(OverrideArgumentsInfos.find(functionName) != OverrideArgumentsInfos.end())
                    OverrideArgumentsInfos.erase(functionName);
            }
            
            inline void ClearAllOverrideArgs()
            {
                OverrideArgumentsInfos.clear();
            }

            #define FO_LOG_CheckOverrideAndSetArgs 0

            template<typename... Args>
            inline bool Internal_CheckOverrideAndSetArgs(std::string functionName, Args&... args)
            {
                #if FO_LOG_CheckOverrideAndSetArgs
                    std::cout << "CheckOverrideAndSetArgs\n";
                    std::cout << "functionName: "<<functionName << "\n";
                #endif
                if(OverrideArgumentsInfos.find(functionName) == OverrideArgumentsInfos.end())
                    return false;
            
                int correctDataIndex = GetCorrectArgumentsDataInfo(functionName, args...);
                
                std::vector<void*> argumentsList;
                AppendArguments(argumentsList, args...);
                
                //Called correctly action
                bool returnResult = false;
                if(correctDataIndex != -1)
                {
                    ArgumentsData& correctData = OverrideArgumentsInfos[functionName].ArgumentsDatas[correctDataIndex];
                    correctData.ArgumentsConditionInfo.CalledTimes++;
                    
                    if(correctData.ArgumentsActionInfo.CorrectActionSet)
                        correctData.ArgumentsActionInfo.CorrectAction(argumentsList);

                    ModifyArgs(argumentsList, correctData.ArgumentsDataInfo, 0, args...);
                    returnResult = true;
                }

                //Deallocating argumentsList
                //for(int i = 0; i < argumentsList.size(); i++)
                //    argumentsList[i].Destructor(argumentsList[i].ArgData);

                return returnResult;
            }
            
            #define FO_ARGUMENTS_IF_FOUND(functionOverrideObj, functionSig, ...)\
            functionOverrideObj.CheckOverrideAndSetArgs(functionSig, __VA_ARGS__)
            
            #define FO_ARGUMENTS_AND_RETURN_IF_FOUND(returnValue, functionOverrideObj, functionSig, ...)\
            do\
            {\
                if(functionOverrideObj.CheckOverrideAndSetArgs(functionSig, __VA_ARGS__))\
                    return returnValue;\
            } while(0)
            
            #define FO_DECLARE_INSTNACE(OverrideObjName) mutable SimpleOverride::FunctionOverrides OverrideObjName
            
            #define FO_DECLARE_OVERRIDE_METHODS(OverrideObjName)\
            inline SimpleOverride::FunctionOverridesArgumentsProxy Internal_OverrideArgs(std::string functionName)\
            {\
                return OverrideObjName.Internal_OverrideArgs(functionName);\
            }\
            inline void Internal_ClearOverrideArgs(std::string functionName)\
            {\
                OverrideObjName.Internal_ClearOverrideArgs(functionName);\
            }\
            inline void ClearAllOverrideArgs()\
            {\
                OverrideObjName.ClearAllOverrideArgs();\
            }\
            inline SimpleOverride::FunctionOverridesReturnProxy Internal_OverrideReturns(std::string functionName)\
            {\
                return OverrideObjName.Internal_OverrideReturns(functionName);\
            }\
            inline void Internal_ClearOverrideReturns(std::string functionName)\
            {\
                OverrideObjName.Internal_ClearOverrideReturns(functionName);\
            }\
            inline void ClearAllOverrideReturns()\
            {\
                OverrideObjName.ClearAllOverrideReturns();\
            }
    };
    
    //==============================================================================
    //Implementation of proxy classes for method chaining
    //==============================================================================
    using ReturnProxy = FunctionOverridesReturnProxy;
    using ArgsProxy = FunctionOverridesArgumentsProxy;
    
    //NOTE: Can't do using as this will evaluate the DeriveType which contains FunctionOverridesCommonProxy which has functions not defined yet.
    //      See https://stackoverflow.com/questions/35428422/crtp-accessing-incomplete-type-members
    //template<typename DeriveType>
    //using CommonProxy = FunctionOverridesCommonProxy<DeriveType>;

    template<typename T>
    inline ReturnProxy& ReturnProxy::ReturnsByAction(std::function<void(std::vector<void*>& args, void* out)> returnAction)
    {
        return FunctionOverridesObj.ReturnsByAction<T>(*this, returnAction);
    }

    template<typename T>
    inline ReturnProxy& ReturnProxy::Returns(T returnData)
    {
        return FunctionOverridesObj.Returns(*this, returnData);
    }
    
    template<typename T>
    inline ArgsProxy& ArgsProxy::SetArgsByAction(std::function<void(std::vector<void*>& args, void* out)> setArgsAction)
    {
        return FunctionOverridesObj.SetArgsByAction<T>(*this, setArgsAction);
    }
            
    template<typename... Args>
    inline ArgsProxy& ArgsProxy::SetArgs(Args... args)
    {
        return FunctionOverridesObj.SetArgs(*this, args...);
    }
    
    template<typename DeriveType>
    inline DeriveType& FunctionOverridesCommonProxy<DeriveType>::Times(int times)
    {
        return FunctionOverridesObj.Times(*this, times);
    }
    
    template<typename DeriveType>
    template<typename... Args>
    inline DeriveType& FunctionOverridesCommonProxy<DeriveType>::WhenCalledWith(Args... args)
    {
        return FunctionOverridesObj.WhenCalledWith(*this, args...);
    }
    
    template<typename DeriveType>
    inline DeriveType& FunctionOverridesCommonProxy<DeriveType>::If(std::function<bool(std::vector<void*>& args)> condition)
    {
        return FunctionOverridesObj.If(*this, condition);
    }
    
    template<typename DeriveType>
    inline DeriveType& FunctionOverridesCommonProxy<DeriveType>::Otherwise_Do(std::function<void(std::vector<void*>& args)> action)
    {
        return FunctionOverridesObj.Otherwise_Do(*this, action);
    }
    
    template<typename DeriveType>
    inline DeriveType& FunctionOverridesCommonProxy<DeriveType>::WhenCalledExpectedly_Do(std::function<void(std::vector<void*>& args)> action)
    {
        return FunctionOverridesObj.WhenCalledExpectedly_Do(*this, action);
    }
    
    //==============================================================================
    //Macros and functions for translating function signature to string
    //==============================================================================
    inline std::string ProcessFunctionSig(std::string functionSig)
    {
        //Trimming
        int firstCharIndex = -1;
        for(int i = 0; i < functionSig.size(); i++)
        {
            if(functionSig[i] != ' ')
            {
                firstCharIndex = i;
                break;
            }
        }
        
        int lastCharIndex = -1;
        for(int i = functionSig.size() - 1; i >= 0; i--)
        {
            if(functionSig[i] != ' ')
            {
                lastCharIndex = i;
                break;
            }
        }
        
        functionSig = functionSig.substr(firstCharIndex, lastCharIndex - firstCharIndex + 1);
        
        //Remove any consecutive spaces
        std::set<int> spacesToRemove;
        bool isLastSpace = false;
        for(int i = 0; i < functionSig.size(); i++)
        {
            if(isLastSpace)
            {
                if(functionSig[i] == ' ')
                    spacesToRemove.insert(i);
                else
                    isLastSpace = false;
            }
            else if(functionSig[i] == ' ')
                isLastSpace = true;
        }
        
        //Remove spaces around parentheses and commas
        for(int i = 0; i < functionSig.size(); i++)
        {
            switch(functionSig[i])
            {
                case '(':
                case ')':
                case ',':
                case '*':
                case '&':
                case ':':
                case '<':
                case '>':
                    if(i > 0 && functionSig[i - 1] == ' ')
                        spacesToRemove.insert(i - 1);
                    
                    if(i < functionSig.size() - 1 && functionSig[i + 1] == ' ')
                        spacesToRemove.insert(i + 1);
                    break;
                default:
                    break;
            }
        }
        
        
        for(auto it = spacesToRemove.rbegin(); it != spacesToRemove.rend(); it++)
            functionSig.erase(functionSig.begin() + *it);

        return functionSig;
    }
    
    #define FO_INTERNAL_APPEND_ARGS_NOT_EMPTY(...) , __VA_ARGS__
    #define FO_INTERNAL_APPEND_ARGS_EMPTY(...)
    
    //NOTE: Up to 20 arguments
    #define FO_INTERNAL_EXPAND_IF_EMPTY() ,,,,,,,,,,,,,,,,,,,,
    #define FO_INTERNAL_ESCAPE_BRACKET(...) __VA_ARGS__ ()
    #define FO_INTERNAL_TEST_EMPTY(...)   FO_INTERNAL_CAT_ALLOW_GARBAGE(FO_INTERNAL_EXPAND_IF_EMPTY, FO_INTERNAL_ESCAPE_BRACKET __VA_ARGS__ ())
    #define FO_INTERNAL_SELECT_TAG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _TAG, ...) _TAG
    #define FO_INTERNAL_CAT_ALLOW_GARBAGE(a, b, ...) a b
    #define FO_INTERNAL_CAT(a, b) a b
    #define FO_INTERNAL_FUNC_CAT(a, b) a ## b
    
    #define FO_INTERNAL_GET_TAG(...) FO_INTERNAL_CAT( FO_INTERNAL_SELECT_TAG, (FO_INTERNAL_TEST_EMPTY(__VA_ARGS__), _EMPTY, \
                                                            _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY,\
                                                            _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY, _NOT_EMPTY) )
    
    //Debug
    //#define FO_INTERNAL_GET_TAG(...) FO_INTERNAL_CAT( FO_INTERNAL_SELECT_TAG, ( FO_INTERNAL_EXPAND_IF_EMPTY __VA_ARGS__ (), _0,
    //                                                    _20, _19, _18, _17, _16, _15, _14, _13, _12, _11,
    //                                                    _10, _9, _8, _7, _6, _5, _4, _3, _2, _1) )
    
    //MSVC workaround: https://stackoverflow.com/questions/48710758/how-to-fix-variadic-macro-related-issues-with-macro-overloading-in-msvc-mic

    #ifndef _MSC_VER
        #define FO_INTERNAL_APPEND_ARGS(...) FO_INTERNAL_CAT( FO_INTERNAL_FUNC_CAT, (FO_INTERNAL_APPEND_ARGS, FO_INTERNAL_GET_TAG(__VA_ARGS__) (__VA_ARGS__)) )
    #else
        #define FO_INTERNAL_VA_ARGS_FIX( macro, args ) macro args
        #define FO_INTERNAL_APPEND_ARGS(...) FO_INTERNAL_VA_ARGS_FIX( FO_INTERNAL_CAT, ( FO_INTERNAL_FUNC_CAT, (FO_INTERNAL_APPEND_ARGS, FO_INTERNAL_GET_TAG(__VA_ARGS__) (__VA_ARGS__)) ) )
    #endif

    #define FO_INTERNAL_STR(x) #x
    
    
    #define OverrideReturns(functionSig) Internal_OverrideReturns(SimpleOverride::ProcessFunctionSig(FO_INTERNAL_STR(functionSig)))
    #define ClearOverrideReturns(functionSig) Internal_ClearOverrideReturns(SimpleOverride::ProcessFunctionSig(FO_INTERNAL_STR(functionSig)))
    #define CheckOverrideAndReturn(returnRef, functionSig, ...)\
        Internal_CheckOverrideAndReturn(returnRef, SimpleOverride::ProcessFunctionSig(FO_INTERNAL_STR(functionSig)) FO_INTERNAL_APPEND_ARGS(__VA_ARGS__) )
    
    #define OverrideArgs(functionSig) Internal_OverrideArgs(SimpleOverride::ProcessFunctionSig(FO_INTERNAL_STR(functionSig)))
    #define ClearOverrideArgs(functionSig) Internal_ClearOverrideArgs(SimpleOverride::ProcessFunctionSig(FO_INTERNAL_STR(functionSig)))
    #define CheckOverrideAndSetArgs(functionSig, ...)\
        Internal_CheckOverrideAndSetArgs(SimpleOverride::ProcessFunctionSig(FO_INTERNAL_STR(functionSig)) FO_INTERNAL_APPEND_ARGS(__VA_ARGS__) )
}

const SimpleOverride::Any FO_ANY;
const SimpleOverride::Any FO_DONT_SET;

template<typename T>
using FO_NonCopyable = SimpleOverride::NonCopyable<T>;
    
template<typename T>
using FO_NonComparable = SimpleOverride::NonComparable<T>;

template<typename T>
using FO_NonComparableCopyable = SimpleOverride::NonComparableCopyable<T>;

#endif
