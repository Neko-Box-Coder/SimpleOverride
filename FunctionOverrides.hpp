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
    struct NonCopyable : public Any
    {
        T* ReferenceVar = nullptr;
    
        NonCopyable(){}
        NonCopyable(T& referenceVar) { ReferenceVar = &referenceVar; }
    };
    
    template<typename T>
    using NonComparable = NonCopyable<T>;
    
    //==============================================================================
    //Method Chaining Classes
    //==============================================================================
    //Common proxy class for method chaining
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

            FunctionOverridesCommonProxy Times(int times);
            
            template<typename... Args>
            FunctionOverridesCommonProxy WhenCalledWith(Args... args);
            
            FunctionOverridesCommonProxy If(std::function<bool(std::vector<ArgInfo>& args)> condition);

            FunctionOverridesCommonProxy Otherwise_Do(std::function<void(std::vector<ArgInfo>& args)> action);

            FunctionOverridesCommonProxy WhenCalledExpectedly_Do(std::function<void(std::vector<ArgInfo>& args)> action);
    };
    
    //Override return proxy class for method chaining
    class FunctionOverridesReturnProxy : public FunctionOverridesCommonProxy
    {
        public:
            FunctionOverridesReturnProxy(std::string functionSignatureName, FunctionOverrides& functionOverridesObj, ProxyType functionProxyType) : 
                FunctionOverridesCommonProxy(functionSignatureName, functionOverridesObj, functionProxyType) 
            {}
            
            template<typename T>
            FunctionOverridesReturnProxy ReturnsByAction(std::function<void(std::vector<ArgInfo>& args, void* out)> returnAction);
            
            template<typename T>
            FunctionOverridesReturnProxy Returns(T returnData);
    };
    
    //Override arguments proxy class for method chaining
    class FunctionOverridesArgumentsProxy : public FunctionOverridesCommonProxy
    {
        public:
            FunctionOverridesArgumentsProxy(std::string functionSignatureName, FunctionOverrides& functionOverridesObj, ProxyType functionProxyType) : 
                FunctionOverridesCommonProxy(functionSignatureName, functionOverridesObj, functionProxyType) 
            {}
            
            template<typename T>
            FunctionOverridesArgumentsProxy SetArgsByAction(std::function<void(std::vector<ArgInfo>& args, void* out)> setArgsAction);
            
            template<typename... Args>
            FunctionOverridesArgumentsProxy SetArgs(Args... args);
    };

    //==============================================================================
    //Main Class
    //==============================================================================
    class FunctionOverrides
    {
        friend class FunctionOverridesCommonProxy;
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
                std::function<void(void*)> Destructor;
                std::function<void(std::vector<ArgInfo>& args, void* out)> DataAction;
                bool DataSet = false;
                bool DataActionSet = false;
            };

            struct ConditionInfo
            {
                std::function<bool(std::vector<ArgInfo>& args)> DataCondition;
                std::vector<ArgInfo> ArgsCondition = {};
                int Times = -1;
                int CalledTimes = 0;
                bool DataConditionSet = false;
            };

            struct ActionInfo
            {
                std::function<void(std::vector<ArgInfo>& args)> OtherwiseAction;
                std::function<void(std::vector<ArgInfo>&)> CalledAction;
                bool OtherwiseActionSet = false;
                bool CalledActionSet = false;
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
            inline FunctionOverridesReturnProxy ReturnsByAction(FunctionOverridesReturnProxy proxy, 
                                                                std::function<void(std::vector<ArgInfo>& args, void* out)> returnAction)
            {
                ReturnData& lastData = OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back();

                lastData.ReturnDataInfo.DataAction = returnAction;
                lastData.ReturnDataInfo.DataActionSet = true;
                lastData.ReturnDataInfo.DataType = typeid(T).hash_code();
                return proxy;
            }
        
            template<typename T>
            inline FunctionOverridesReturnProxy Returns(FunctionOverridesReturnProxy proxy, T returnData)
            {
                T* returnDataP = static_cast<T*>(malloc(sizeof(T)));

                ReturnData& lastData = OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back();
                lastData.ReturnDataInfo.Data = new T(returnData);
                lastData.ReturnDataInfo.Destructor = [](void* data) { delete static_cast<T*>(data); }; 
                lastData.ReturnDataInfo.DataSet = true;
                lastData.ReturnDataInfo.DataType = typeid(T).hash_code();
                return proxy;
            }
            
            //------------------------------------------------------------------------------
            //Methods for setting arguments data
            //------------------------------------------------------------------------------
            template<typename T>
            inline FunctionOverridesArgumentsProxy SetArgsByAction( FunctionOverridesArgumentsProxy proxy,
                                                                    std::function<void(std::vector<ArgInfo>& args, void* out)> setArgsAction)
            {
                ArgumentsData& lastData = OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back();
                
                lastData.ArgumentsDataInfo.push_back(DataInfo());
                
                lastData.ArgumentsDataInfo.back().DataAction = setArgsAction;
                lastData.ArgumentsDataInfo.back().DataActionSet = true;
                lastData.ArgumentsDataInfo.back().DataType = typeid(T).hash_code();
                return proxy;
            }
            
            inline FunctionOverridesArgumentsProxy SetArgs(FunctionOverridesArgumentsProxy proxy)
            {
                return proxy;
            }
            
            #if 0
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
            inline FunctionOverridesArgumentsProxy SetArgs( FunctionOverridesArgumentsProxy proxy,
                                                            T arg, Args... args)
            {
                ArgumentsData& lastData = OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back();
                lastData.ArgumentsDataInfo.push_back(DataInfo());
                
                if(!std::is_same<T, Any>())
                {
                    lastData.ArgumentsDataInfo.back().Data = new T(arg);
                    lastData.ArgumentsDataInfo.back().Destructor = [](void* data) { delete static_cast<T*>(data); };
                    lastData.ArgumentsDataInfo.back().DataSet = true;
                    lastData.ArgumentsDataInfo.back().DataType = typeid(T).hash_code();

                    #if 0
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
            
            
            //------------------------------------------------------------------------------
            //Methods for setting requirements
            //------------------------------------------------------------------------------
            inline FunctionOverridesCommonProxy Times(FunctionOverridesCommonProxy proxy, int times)
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
                return proxy;
            }
            
            inline FunctionOverridesCommonProxy WhenCalledWith(FunctionOverridesCommonProxy proxy)
            {
                return proxy;
            }

            template<typename T, typename... Args>
            inline FunctionOverridesCommonProxy WhenCalledWith(FunctionOverridesCommonProxy proxy, T arg, Args... args)
            {
                ArgInfo curArg;
                if(!std::is_same<T, Any>())
                {
                    curArg.ArgData = new T(arg);
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
            
            inline FunctionOverridesCommonProxy If( FunctionOverridesCommonProxy proxy, 
                                                    std::function<bool(std::vector<ArgInfo>& args)> condition)
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
            
                return proxy;
            }
            
            inline FunctionOverridesCommonProxy Otherwise_Do(   FunctionOverridesCommonProxy proxy, 
                                                                std::function<void(std::vector<ArgInfo>& args)> action)
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
                
                return proxy;
            }
            
            inline FunctionOverridesCommonProxy WhenCalledExpectedly_Do(FunctionOverridesCommonProxy proxy, 
                                                                        std::function<void(std::vector<ArgInfo>& args)> action)
            {
                switch(proxy.FunctionProxyType)
                {
                    case ProxyType::RETURN:
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnActionInfo.CalledAction = action;
                        OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back().ReturnActionInfo.CalledActionSet = true;
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsActionInfo.CalledAction = action;
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].ArgumentsDatas.back().ArgumentsActionInfo.CalledActionSet = true;
                        break;
                    case ProxyType::COMMON:
                        std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                        assert(false);
                        exit(1);
                        break;
                }
    
                return proxy;
            }

        //==============================================================================
        //Implementation methods for querying (and setting) the correct return or arugment data
        //==============================================================================
        private:
            inline void AppendArguments(std::vector<ArgInfo>& argumentsList){};

            template<typename T, typename... Args>
            inline void AppendArguments(std::vector<ArgInfo>& argumentsList, T arg, Args... args)
            {
                ArgInfo curArgInfo;
                curArgInfo.ArgData = new T(arg);
                curArgInfo.Destructor = [](void* data){ delete static_cast<T*>(data); };
                curArgInfo.ArgSize = sizeof(T);
                curArgInfo.ArgTypeHash = typeid(T).hash_code();
                curArgInfo.ArgSet = true;
                
                argumentsList.push_back(curArgInfo);
                AppendArguments(argumentsList, args...);
            }
            
            inline void AppendArgumentsDereference(std::vector<ArgInfo>& argumentsList) {}

            template<typename T, typename... Args>
            inline void AppendArgumentsDereference(std::vector<ArgInfo>& argumentsList, T& arg, Args&... args)
            {
                ArgInfo curArgInfo;
                curArgInfo.ArgData = new INTERNAL_FO_PURE_T(arg);
                curArgInfo.Destructor = [](void* data){ delete static_cast<INTERNAL_FO_PURE_T*>(data); };
                curArgInfo.ArgSize = sizeof(INTERNAL_FO_PURE_T);
                curArgInfo.ArgTypeHash = typeid(INTERNAL_FO_PURE_T).hash_code();
                curArgInfo.ArgSet = true;
                
                argumentsList.push_back(curArgInfo);
                AppendArgumentsDereference(argumentsList, args...);
            }
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline void AppendArgumentsDereference(std::vector<ArgInfo>& argumentsList, T* arg, Args&... args)
            {
                AppendArgumentsDereference(argumentsList, *arg, args...);
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
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, T* arg, Args&... args)
            {
                return CheckArguments(argumentsListToCheck, argIndex, *arg, args...);
            }
            
            #define FO_LOG_GetCorrectReturnDataInfo 0
            template<typename T, typename FUNC_SIG, typename... Args>
            inline int GetCorrectReturnDataInfo(T& returnRef, FUNC_SIG functionSignature, Args... args)
            {
                std::string functionSignatureString = typeid(FUNC_SIG).name();
                if(OverrideReturnInfos.find(functionSignatureString) == OverrideReturnInfos.end())
                {
                    std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                    assert(false);
                    exit(1);
                }
                
                #if FO_LOG_GetCorrectReturnDataInfo
                    std::cout <<"GetCorrectReturnDataInfo called\n";
                #endif

                std::vector<ArgInfo> argumentsList;
                AppendArguments(argumentsList, args...);
                
                std::vector<ReturnData>& curReturnDatas = OverrideReturnInfos[functionSignatureString].ReturnDatas;
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
                            curReturnDatas[i].ReturnActionInfo.CalledAction(argumentsList);
                        continue;
                    }
                
                    //Check return type
                    if(curReturnDatas[i].ReturnDataInfo.DataType != typeid(INTERNAL_FO_PURE_T).hash_code())
                    {
                        #if FO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at return type\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.CalledAction(argumentsList);
                        
                        continue;
                    }

                    //Check condition
                    if(curReturnDatas[i].ReturnConditionInfo.DataConditionSet && !curReturnDatas[i].ReturnConditionInfo.DataCondition(argumentsList))
                    {
                        #if FO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at Check condition\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.CalledAction(argumentsList);
                        
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
                            curReturnDatas[i].ReturnActionInfo.CalledAction(argumentsList);

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
                            curReturnDatas[i].ReturnActionInfo.CalledAction(argumentsList);

                        continue;
                    }
                    
                    #if FO_LOG_GetCorrectReturnDataInfo
                        std::cout << "Return data found: "<<i<<"\n";
                    #endif
                    returnIndex = i;
                    break;
                }
                
                //Deallocating argumentsList
                for(int i = 0; i < argumentsList.size(); i++)
                    argumentsList[i].Destructor(argumentsList[i].ArgData);
                
                return returnIndex;
            }
            
            template<typename FUNC_SIG, typename... Args>
            inline int GetCorrectArgumentsDataInfo(FUNC_SIG functionSignature, Args&... args)
            {
                std::string functionSignatureString = typeid(FUNC_SIG).name();
                if(OverrideArgumentsInfos.find(functionSignatureString) == OverrideArgumentsInfos.end())
                {
                    std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                    assert(false);
                    exit(1);
                }
                
                #if 0
                    std::cout <<"GetCorrectArgumentsDataInfo called\n";
                #endif
                
                std::vector<ArgInfo> argumentsList;
                AppendArguments(argumentsList, args...);
                
                std::vector<ArgInfo> derefArgumentsList;
                AppendArgumentsDereference(derefArgumentsList, args...);
                
                std::vector<ArgumentsData>& curArgData = OverrideArgumentsInfos[functionSignatureString].ArgumentsDatas;
                int returnIndex = -1;
                for(int i = 0; i < curArgData.size(); i++)
                {
                    #if 0
                        std::cout << "Checking arg data["<<i<<"]\n";
                    #endif
                
                    //Check set argument data exist
                    if(curArgData[i].ArgumentsDataInfo.size() != derefArgumentsList.size())
                    {
                        #if 0
                            std::cout << "Failed at Check set argument data exist\n";
                        #endif
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.CalledAction(argumentsList);
                        
                        continue;
                    }
                
                    //Check arguments types
                    bool argumentTypeFailed = false;
                    for(int j = 0; j < curArgData[i].ArgumentsDataInfo.size(); j++)
                    {
                        if( curArgData[i].ArgumentsDataInfo[j].DataSet &&
                            curArgData[i].ArgumentsDataInfo[j].DataType != derefArgumentsList[j].ArgTypeHash)
                        {
                            #if 0
                                std::cout << "Failed at Check arguments types\n";
                            #endif
                            argumentTypeFailed = true;
                            break;;
                        }
                    }
                    if(argumentTypeFailed)
                    {
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.CalledAction(argumentsList);

                        continue;
                    }

                    //Check condition
                    if(curArgData[i].ArgumentsConditionInfo.DataConditionSet && !curArgData[i].ArgumentsConditionInfo.DataCondition(argumentsList))
                    {
                        #if 0
                            std::cout << "Failed at Check condition\n";
                        #endif
                        continue;
                    }

                    //Check parameter
                    if( !curArgData[i].ArgumentsConditionInfo.ArgsCondition.empty() && 
                        !CheckArguments(curArgData[i].ArgumentsConditionInfo.ArgsCondition, 0, args...))
                    {
                        #if 0
                            std::cout << "Failed at Check parameter\n";
                        #endif
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.CalledAction(argumentsList);
                        continue;
                    }
                        
                    //Check times
                    if( curArgData[i].ArgumentsConditionInfo.Times >= 0 && 
                        curArgData[i].ArgumentsConditionInfo.CalledTimes >= curArgData[i].ArgumentsConditionInfo.Times)
                    {
                        #if 0
                            std::cout << "Failed at Check times\n";
                        #endif
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.CalledAction(argumentsList);
                        continue;
                    }
                    
                    #if 0
                        std::cout << "Argument data found: "<<i<<"\n";
                    #endif
                    
                    returnIndex = i;
                    break;
                }
                
                //Deallocating argumentsList
                for(int i = 0; i < argumentsList.size(); i++)
                    argumentsList[i].Destructor(argumentsList[i].ArgData);
                
                //Deallocating derefArgumentsList
                for(int i = 0; i < derefArgumentsList.size(); i++)
                    derefArgumentsList[i].Destructor(derefArgumentsList[i].ArgData);

                return returnIndex;
            }
            
            inline void ModifyArgs(std::vector<ArgInfo>& argumentsList, std::vector<DataInfo>& argsData, int index) {}

            template<typename T, typename... Args>
            inline void ModifyArgs(std::vector<ArgInfo>& argumentsList, std::vector<DataInfo>& argsData, int index, T& arg, Args&... args)
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
                    #if 0
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
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline void ModifyArgs(std::vector<ArgInfo>& argumentsList, std::vector<DataInfo>& argsData, int index, T* arg, Args&... args)
            {
                ModifyArgs(argumentsList, argsData, index, *arg, args...);
            }

        //==============================================================================
        //Public facing methods for overriding returns or arguments
        //==============================================================================
        public:
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
            template<typename FUNC_SIG>
            inline FunctionOverridesReturnProxy OverrideReturns(FUNC_SIG functionName)
            {
                std::string functionNameString = typeid(FUNC_SIG).name();
                
                #if FO_LOG_OverrideReturns
                    std::cout << "OverrideReturns\n";
                    std::cout << "functionNameString: "<<functionNameString << "\n";
                    std::cout << "typeid(FUNC_SIG).hash_code(): "<<typeid(FUNC_SIG).hash_code() << "\n";
                #endif

                OverrideReturnInfos[functionNameString].ReturnDatas.push_back(ReturnData());
                return FunctionOverridesReturnProxy(functionNameString, *this, ProxyType::RETURN);
            }
            
            template<typename FUNC_SIG>
            inline void ClearOverrideReturns(FUNC_SIG functionName)
            {
                std::string functionNameString = typeid(FUNC_SIG).name();

                if(OverrideReturnInfos.find(functionNameString) != OverrideReturnInfos.end())
                    OverrideReturnInfos.erase(functionNameString);
            }
            
            inline void ClearAllOverrideReturns()
            {
                OverrideReturnInfos.clear();
            }
            
            #define FO_LOG_CheckOverrideAndReturn 0
            template<typename T, typename FUNC_SIG, typename... Args>
            inline bool CheckOverrideAndReturn(T& returnRef, FUNC_SIG functionName, Args&... args)
            {
                std::string functionNameString = typeid(FUNC_SIG).name();
                #if FO_LOG_CheckOverrideAndReturn
                    std::cout << "CheckOverrideAndReturn\n";
                    std::cout << "functionNameString: "<<functionNameString << "\n";
                    std::cout << "typeid(FUNC_SIG).hash_code(): "<<typeid(FUNC_SIG).hash_code() << "\n";
                #endif
                if(OverrideReturnInfos.find(functionNameString) == OverrideReturnInfos.end())
                    return false;
            
                int correctDataIndex = GetCorrectReturnDataInfo(returnRef, functionName, args...);
                
                std::vector<ArgInfo> argumentsList;
                AppendArguments(argumentsList, args...);
                
                //Called correctly action
                bool returnResult = false;
                if(correctDataIndex != -1)
                {
                    ReturnData& correctData = OverrideReturnInfos[functionNameString].ReturnDatas[correctDataIndex];
                    correctData.ReturnConditionInfo.CalledTimes++;
                    
                    if(correctData.ReturnActionInfo.CalledActionSet)
                        correctData.ReturnActionInfo.CalledAction(argumentsList);
                    
                    if(correctData.ReturnDataInfo.DataSet)
                        returnRef = *reinterpret_cast<T*>(correctData.ReturnDataInfo.Data);
                    else
                        correctData.ReturnDataInfo.DataAction(argumentsList, &returnRef);

                    assert(correctData.ReturnDataInfo.DataSet || correctData.ReturnDataInfo.DataActionSet);
                    returnResult = true;
                }
                
                //Deallocating argumentsList
                for(int i = 0; i < argumentsList.size(); i++)
                    argumentsList[i].Destructor(argumentsList[i].ArgData);

                return returnResult;
            }
            
            #define FO_RETURN_IF_FOUND_WITHOUT_ARGS(functionOverrideObj, functionRef, returnType)\
            do\
            {\
                returnType returnVal;\
                if(functionOverrideObj.CheckOverrideAndReturn(returnVal, functionRef))\
                    return returnVal;\
            } while(0)
            
            #define FO_RETURN_IF_FOUND(functionOverrideObj, functionRef, returnType, ...)\
            do\
            {\
                returnType returnVal;\
                if(functionOverrideObj.CheckOverrideAndReturn(returnVal, functionRef, __VA_ARGS__))\
                    return returnVal;\
            } while(0)


            //------------------------------------------------------------------------------
            //Overrding Arguments
            //------------------------------------------------------------------------------
            #define FO_LOG_OverrideArgs 0
            template<typename FUNC_SIG>
            inline FunctionOverridesArgumentsProxy OverrideArgs(FUNC_SIG functionName)
            {
                std::string functionNameString = typeid(FUNC_SIG).name();
                #if FO_LOG_OverrideArgs
                    std::cout << "OverrideArgs\n";
                    std::cout << "functionNameString: "<<functionNameString << "\n";
                    std::cout << "typeid(FUNC_SIG).hash_code(): "<<typeid(FUNC_SIG).hash_code() << "\n";
                #endif

                OverrideArgumentsInfos[functionNameString].ArgumentsDatas.push_back(ArgumentsData());
                return FunctionOverridesArgumentsProxy(functionNameString, *this, ProxyType::ARGS);
            }
            
            template<typename FUNC_SIG>
            inline void ClearOverrideArgs(FUNC_SIG functionName)
            {
                std::string functionNameString = typeid(FUNC_SIG).name();

                if(OverrideArgumentsInfos.find(functionNameString) != OverrideArgumentsInfos.end())
                    OverrideArgumentsInfos.erase(functionNameString);
            }
            
            inline void ClearAllOverrideArgs()
            {
                OverrideArgumentsInfos.clear();
            }

            #define FO_LOG_CheckOverrideAndSetArgs 0
            template<typename FUNC_SIG, typename... Args>
            inline bool CheckOverrideAndSetArgs(FUNC_SIG functionName, Args&... args)
            {
                std::string functionNameString = typeid(FUNC_SIG).name();
                #if FO_LOG_CheckOverrideAndSetArgs
                    std::cout << "CheckOverrideAndSetArgs\n";
                    std::cout << "functionNameString: "<<functionNameString << "\n";
                    std::cout << "typeid(FUNC_SIG).hash_code(): "<<typeid(FUNC_SIG).hash_code() << "\n";
                #endif
                if(OverrideArgumentsInfos.find(functionNameString) == OverrideArgumentsInfos.end())
                    return false;
            
                int correctDataIndex = GetCorrectArgumentsDataInfo(functionName, args...);
                
                std::vector<ArgInfo> argumentsList;
                AppendArguments(argumentsList, args...);
                
                //Called correctly action
                bool returnResult = false;
                if(correctDataIndex != -1)
                {
                    ArgumentsData& correctData = OverrideArgumentsInfos[functionNameString].ArgumentsDatas[correctDataIndex];
                    correctData.ArgumentsConditionInfo.CalledTimes++;
                    
                    if(correctData.ArgumentsActionInfo.CalledActionSet)
                        correctData.ArgumentsActionInfo.CalledAction(argumentsList);

                    ModifyArgs(argumentsList, correctData.ArgumentsDataInfo, 0, args...);
                    returnResult = true;
                }

                //Deallocating argumentsList
                for(int i = 0; i < argumentsList.size(); i++)
                    argumentsList[i].Destructor(argumentsList[i].ArgData);

                return returnResult;
            }
            
            #define FO_ARGUMENTS_IF_FOUND(functionOverrideObj, functionRef, ...)\
            functionOverrideObj.CheckOverrideAndSetArgs(functionRef, __VA_ARGS__)
            
            #define FO_ARGUMENTS_AND_RETURN_IF_FOUND(returnValue, functionOverrideObj, functionRef, ...)\
            do\
            {\
                if(functionOverrideObj.CheckOverrideAndSetArgs(functionRef, __VA_ARGS__))\
                    return returnValue;\
            } while(0)
            
            #define FO_DECLARE_INSTNACE(OverrideObjName) mutable SimpleOverride::FunctionOverrides OverrideObjName
            
            #define FO_DECLARE_OVERRIDE_METHODS(OverrideObjName)\
            template<typename FUNC_SIG>\
            inline SimpleOverride::FunctionOverridesArgumentsProxy OverrideArgs(FUNC_SIG functionName)\
            {\
                return OverrideObjName.OverrideArgs(functionName);\
            }\
            template<typename FUNC_SIG>\
            inline void ClearOverrideArgs(FUNC_SIG functionName)\
            {\
                OverrideObjName.ClearOverrideArgs(functionName);\
            }\
            inline void ClearAllOverrideArgs()\
            {\
                OverrideObjName.ClearAllOverrideArgs();\
            }\
            template<typename FUNC_SIG>\
            inline SimpleOverride::FunctionOverridesReturnProxy OverrideReturns(FUNC_SIG functionName)\
            {\
                return OverrideObjName.OverrideReturns(functionName);\
            }\
            template<typename FUNC_SIG>\
            inline void ClearOverrideReturns(FUNC_SIG functionName)\
            {\
                OverrideObjName.ClearOverrideReturns(functionName);\
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
    using CommonProxy = FunctionOverridesCommonProxy;

    template<typename T>
    inline ReturnProxy ReturnProxy::ReturnsByAction(std::function<void(std::vector<ArgInfo>& args, void* out)> returnAction)
    {
        return FunctionOverridesObj.ReturnsByAction<T>(*this, returnAction);
    }

    template<typename T>
    inline ReturnProxy ReturnProxy::Returns(T returnData)
    {
        return FunctionOverridesObj.Returns(*this, returnData);
    }
    
    template<typename T>
    inline ArgsProxy ArgsProxy::SetArgsByAction(std::function<void(std::vector<ArgInfo>& args, void* out)> setArgsAction)
    {
        return FunctionOverridesObj.SetArgsByAction<T>(*this, setArgsAction);
    }
            
    template<typename... Args>
    inline ArgsProxy ArgsProxy::SetArgs(Args... args)
    {
        return FunctionOverridesObj.SetArgs(*this, args...);
    }
    
    inline CommonProxy CommonProxy::Times(int times)
    {
        return FunctionOverridesObj.Times(*this, times);
    }
    
    template<typename... Args>
    inline CommonProxy CommonProxy::WhenCalledWith(Args... args)
    {
        return FunctionOverridesObj.WhenCalledWith(*this, args...);
    }
    
    inline CommonProxy CommonProxy::If(std::function<bool(std::vector<ArgInfo>& args)> condition)
    {
        return FunctionOverridesObj.If(*this, condition);
    }
    
    inline CommonProxy CommonProxy::Otherwise_Do(std::function<void(std::vector<ArgInfo>& args)> action)
    {
        return FunctionOverridesObj.Otherwise_Do(*this, action);
    }
    
    inline CommonProxy CommonProxy::WhenCalledExpectedly_Do(std::function<void(std::vector<ArgInfo>& args)> action)
    {
        return FunctionOverridesObj.WhenCalledExpectedly_Do(*this, action);
    }
}

const SimpleOverride::Any FO_ANY;
const SimpleOverride::Any FO_DONT_SET;

template<typename T>
using FO_NonCopyable = SimpleOverride::NonCopyable<T>;
    
template<typename T>
using FO_NonComparable = SimpleOverride::NonComparable<T>;


#endif
