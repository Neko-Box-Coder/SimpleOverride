#ifndef SIMPLE_OVERRIDE_FUNCTION_OVERRIDES_H
#define SIMPLE_OVERRIDE_FUNCTION_OVERRIDES_H

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

            struct OverrideActionInfo
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
            };

            struct OverrideReturnDataInfo
            {
                std::vector<ReturnData> ReturnDatas = {};
                OverrideActionInfo OverrideReturnInfo;
            };
            
            struct ArgumentsData
            {
                ConditionInfo ArgumentsConditionInfo;
                std::vector<DataInfo> ArgumentsDataInfo;
            };

            struct OverrideArgumentInfo
            {
                std::vector<ArgumentsData> ArgumentsDatas = {};
                OverrideActionInfo OverrideArgumentsInfo;
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
                INTERNAL_FO_PURE_T* returnDataP = static_cast<INTERNAL_FO_PURE_T*>(malloc(sizeof(INTERNAL_FO_PURE_T)));

                ReturnData& lastData = OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back();
                lastData.ReturnDataInfo.Data = new INTERNAL_FO_PURE_T(returnData);
                lastData.ReturnDataInfo.Destructor = [](void* data) { delete static_cast<INTERNAL_FO_PURE_T*>(data); }; 
                lastData.ReturnDataInfo.DataSet = true;
                lastData.ReturnDataInfo.DataType = typeid(INTERNAL_FO_PURE_T).hash_code();
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
                    lastData.ArgumentsDataInfo.back().Data = new INTERNAL_FO_PURE_T(arg);
                    lastData.ArgumentsDataInfo.back().Destructor = [](void* data) { delete static_cast<INTERNAL_FO_PURE_T*>(data); };
                    lastData.ArgumentsDataInfo.back().DataSet = true;
                    lastData.ArgumentsDataInfo.back().DataType = typeid(INTERNAL_FO_PURE_T).hash_code();

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
                    curArg.ArgData = new INTERNAL_FO_PURE_T(arg);
                    curArg.Destructor = [](void* data){ delete static_cast<INTERNAL_FO_PURE_T*>(data); };
                    curArg.ArgSize = sizeof(INTERNAL_FO_PURE_T);
                    curArg.ArgTypeHash = typeid(INTERNAL_FO_PURE_T).hash_code();
                    curArg.ArgSet = true;
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
                        OverrideReturnInfos[proxy.FunctionSignatureName].OverrideReturnInfo.OtherwiseAction = action;
                        OverrideReturnInfos[proxy.FunctionSignatureName].OverrideReturnInfo.OtherwiseActionSet = true;
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].OverrideArgumentsInfo.OtherwiseAction = action;
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].OverrideArgumentsInfo.OtherwiseActionSet = true;
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
                        OverrideReturnInfos[proxy.FunctionSignatureName].OverrideReturnInfo.CalledAction = action;
                        OverrideReturnInfos[proxy.FunctionSignatureName].OverrideReturnInfo.CalledActionSet = true;
                        break;
                    case ProxyType::ARGS:
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].OverrideArgumentsInfo.CalledAction = action;
                        OverrideArgumentsInfos[proxy.FunctionSignatureName].OverrideArgumentsInfo.CalledActionSet = true;
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
                curArgInfo.ArgData = new INTERNAL_FO_PURE_T(arg);
                curArgInfo.Destructor = [](void* data){ delete static_cast<INTERNAL_FO_PURE_T*>(data); };
                curArgInfo.ArgSize = sizeof(INTERNAL_FO_PURE_T);
                curArgInfo.ArgTypeHash = typeid(INTERNAL_FO_PURE_T).hash_code();
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

            template<typename T, typename... Args>
            inline bool CheckArguments(std::vector<ArgInfo>& argumentsListToCheck, int argIndex, T& arg, Args&... args)
            {
                if(argIndex >= argumentsListToCheck.size())
                    return false;

                if(argumentsListToCheck[argIndex].ArgSet)
                {
                    if(sizeof(INTERNAL_FO_PURE_T) != argumentsListToCheck[argIndex].ArgSize)
                        return false;
                        
                    if(typeid(INTERNAL_FO_PURE_T).hash_code() != argumentsListToCheck[argIndex].ArgTypeHash)
                        return false;

                    if(arg != *reinterpret_cast<INTERNAL_FO_PURE_T*>(argumentsListToCheck[argIndex].ArgData))
                        return false;
                }            
                
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
                
                std::vector<ArgInfo> argumentsList;
                AppendArguments(argumentsList, args...);
                
                std::vector<ReturnData>& curReturnDatas = OverrideReturnInfos[functionSignatureString].ReturnDatas;
                int returnIndex = -1;
                for(int i = 0; i < curReturnDatas.size(); i++)
                {
                    //Check return data exist
                    if(!curReturnDatas[i].ReturnDataInfo.DataSet && !curReturnDatas[i].ReturnDataInfo.DataActionSet)
                        continue;
                
                    //Check return type
                    if(curReturnDatas[i].ReturnDataInfo.DataType != typeid(INTERNAL_FO_PURE_T).hash_code())
                        continue;

                    //Check condition
                    if(curReturnDatas[i].ReturnConditionInfo.DataConditionSet && !curReturnDatas[i].ReturnConditionInfo.DataCondition(argumentsList))
                        continue;

                    //Check parameter
                    if( !curReturnDatas[i].ReturnConditionInfo.ArgsCondition.empty() && 
                        !CheckArguments(curReturnDatas[i].ReturnConditionInfo.ArgsCondition, 0, args...))
                    {
                        continue;
                    }
                        
                    //Check times
                    if( curReturnDatas[i].ReturnConditionInfo.Times >= 0 && 
                        curReturnDatas[i].ReturnConditionInfo.CalledTimes >= curReturnDatas[i].ReturnConditionInfo.Times)
                    {
                        continue;
                    }
                    
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
                
                std::vector<ArgInfo> argumentsList;
                AppendArguments(argumentsList, args...);
                
                std::vector<ArgInfo> derefArgumentsList;
                AppendArgumentsDereference(derefArgumentsList, args...);
                
                std::vector<ArgumentsData>& curArgData = OverrideArgumentsInfos[functionSignatureString].ArgumentsDatas;
                int returnIndex = -1;
                for(int i = 0; i < curArgData.size(); i++)
                {
                    //Check set argument data exist
                    if(curArgData[i].ArgumentsDataInfo.size() != derefArgumentsList.size())
                        continue;
                
                    //Check arguments types
                    for(int j = 0; j < curArgData[i].ArgumentsDataInfo.size(); j++)
                    {
                        if( curArgData[i].ArgumentsDataInfo[j].DataSet &&
                            curArgData[i].ArgumentsDataInfo[j].DataType != derefArgumentsList[j].ArgTypeHash)
                        {
                            continue;
                        }
                    }

                    //Check condition
                    if(curArgData[i].ArgumentsConditionInfo.DataConditionSet && !curArgData[i].ArgumentsConditionInfo.DataCondition(argumentsList))
                        continue;

                    //Check parameter
                    if( !curArgData[i].ArgumentsConditionInfo.ArgsCondition.empty() && 
                        !CheckArguments(curArgData[i].ArgumentsConditionInfo.ArgsCondition, 0, args...))
                    {
                        continue;
                    }
                        
                    //Check times
                    if( curArgData[i].ArgumentsConditionInfo.Times >= 0 && 
                        curArgData[i].ArgumentsConditionInfo.CalledTimes >= curArgData[i].ArgumentsConditionInfo.Times)
                    {
                        continue;
                    }
                    
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
                    arg = *static_cast<T*>(argsData[index].Data);
                    
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
                    argsData[index].DataAction(argumentsList, &arg);

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
            template<typename FUNC_SIG>
            inline FunctionOverridesReturnProxy OverrideReturns(FUNC_SIG functionSignature)
            {
                std::string functionSignatureString = typeid(FUNC_SIG).name();

                OverrideReturnInfos[functionSignatureString].ReturnDatas.push_back(ReturnData());
                return FunctionOverridesReturnProxy(functionSignatureString, *this, ProxyType::RETURN);
            }
            
            template<typename T, typename FUNC_SIG, typename... Args>
            inline bool CheckOverrideAndReturn(T& returnRef, FUNC_SIG functionSignature, Args&... args)
            {
                std::string functionSignatureString = typeid(FUNC_SIG).name();
                if(OverrideReturnInfos.find(functionSignatureString) == OverrideReturnInfos.end())
                    return false;
            
                int correctDataIndex = GetCorrectReturnDataInfo(returnRef, functionSignature, args...);
                
                std::vector<ArgInfo> argumentsList;
                AppendArguments(argumentsList, args...);
                
                //Called correctly action
                bool returnResult = false;
                if(correctDataIndex != -1)
                {
                    ReturnData& correctData = OverrideReturnInfos[functionSignatureString].ReturnDatas[correctDataIndex];
                    correctData.ReturnConditionInfo.CalledTimes++;
                    
                    if(OverrideReturnInfos[functionSignatureString].OverrideReturnInfo.CalledActionSet)
                        OverrideReturnInfos[functionSignatureString].OverrideReturnInfo.CalledAction(argumentsList);
                    
                    if(correctData.ReturnDataInfo.DataSet)
                        returnRef = *reinterpret_cast<T*>(correctData.ReturnDataInfo.Data);
                    else
                        correctData.ReturnDataInfo.DataAction(argumentsList, &returnRef);

                    assert(correctData.ReturnDataInfo.DataSet || correctData.ReturnDataInfo.DataActionSet);
                    returnResult = true;
                }
                else
                {
                    //Otherwise action
                    if(OverrideReturnInfos[functionSignatureString].OverrideReturnInfo.OtherwiseActionSet)
                        OverrideReturnInfos[functionSignatureString].OverrideReturnInfo.OtherwiseAction(argumentsList);
                    
                    returnResult = false;
                }
                
                //Deallocating argumentsList
                for(int i = 0; i < argumentsList.size(); i++)
                    argumentsList[i].Destructor(argumentsList[i].ArgData);

                return returnResult;
            }
            
            #define FO_RETURN_IF_FOUND_WITHOUT_ARGS(functionOverrideObj, funcSignature, returnType)\
            do\
            {\
                returnType returnVal;\
                if(functionOverrideObj.CheckOverrideAndReturn(returnVal, funcSignature))\
                    return returnVal;\
            } while(0)
            
            #define FO_RETURN_IF_FOUND(functionOverrideObj, funcSignature, returnType, ...)\
            do\
            {\
                returnType returnVal;\
                if(functionOverrideObj.CheckOverrideAndReturn(returnVal, funcSignature, __VA_ARGS__))\
                    return returnVal;\
            } while(0)


            //------------------------------------------------------------------------------
            //Overrding Arguments
            //------------------------------------------------------------------------------
            template<typename FUNC_SIG>
            inline FunctionOverridesArgumentsProxy OverrideArgs(FUNC_SIG functionSignature)
            {
                std::string functionSignatureString = typeid(FUNC_SIG).name();

                OverrideArgumentsInfos[functionSignatureString].ArgumentsDatas.push_back(ArgumentsData());
                return FunctionOverridesArgumentsProxy(functionSignatureString, *this, ProxyType::ARGS);
            }

            template<typename FUNC_SIG, typename... Args>
            inline bool CheckOverrideAndSetArgs(FUNC_SIG functionSignature, Args&... args)
            {
                std::string functionSignatureString = typeid(FUNC_SIG).name();
                if(OverrideArgumentsInfos.find(functionSignatureString) == OverrideArgumentsInfos.end())
                    return false;
            
                int correctDataIndex = GetCorrectArgumentsDataInfo(functionSignature, args...);
                
                std::vector<ArgInfo> argumentsList;
                AppendArguments(argumentsList, args...);
                
                //Called correctly action
                bool returnResult = false;
                if(correctDataIndex != -1)
                {
                    ArgumentsData& correctData = OverrideArgumentsInfos[functionSignatureString].ArgumentsDatas[correctDataIndex];
                    correctData.ArgumentsConditionInfo.CalledTimes++;
                    
                    if(OverrideArgumentsInfos[functionSignatureString].OverrideArgumentsInfo.CalledActionSet)
                        OverrideArgumentsInfos[functionSignatureString].OverrideArgumentsInfo.CalledAction(argumentsList);

                    ModifyArgs(argumentsList, correctData.ArgumentsDataInfo, 0, args...);
                    returnResult = true;
                }
                else
                {
                    //Otherwise action
                    if(OverrideArgumentsInfos[functionSignatureString].OverrideArgumentsInfo.OtherwiseActionSet)
                        OverrideArgumentsInfos[functionSignatureString].OverrideArgumentsInfo.OtherwiseAction(argumentsList);

                    returnResult = false;
                }
                
                //Deallocating argumentsList
                for(int i = 0; i < argumentsList.size(); i++)
                    argumentsList[i].Destructor(argumentsList[i].ArgData);

                return returnResult;
            }
            
            #define FO_ARGUMENTS_IF_FOUND(functionOverrideObj, funcSignature, ...)\
            functionOverrideObj.CheckOverrideAndSetArgs(funcSignature, __VA_ARGS__)
            
            #define FO_ARGUMENTS_AND_RETURN_IF_FOUND(returnValue, functionOverrideObj, funcSignature, ...)\
            do\
            {\
                if(functionOverrideObj.CheckOverrideAndSetArgs(funcSignature, __VA_ARGS__))\
                    return returnValue;\
            } while(0)
            
            #define FO_DECLARE_INSTNACE(functionOverrideName) mutable SimpleOverride::FunctionOverrides functionOverrideName
            
            #define FO_DECLARE_OVERRIDE_METHODS(functionOverrideName)\
            template<typename FUNC_SIG>\
            inline SimpleOverride::FunctionOverridesArgumentsProxy OverrideArgs(FUNC_SIG functionSignature)\
            {\
                return functionOverrideName.OverrideArgs(functionSignature);\
            }\
            template<typename FUNC_SIG>\
            inline SimpleOverride::FunctionOverridesReturnProxy OverrideReturns(FUNC_SIG functionSignature)\
            {\
                return functionOverrideName.OverrideReturns(functionSignature);\
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

#endif
