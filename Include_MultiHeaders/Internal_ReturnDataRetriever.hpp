#ifndef SO_INTERNAL_RETURN_DATA_RETRIEVER_HPP
#define SO_INTERNAL_RETURN_DATA_RETRIEVER_HPP

#include "./Internal_OverrideReturnDataInfo.hpp"
#include "./PureType.hpp"
#include "./Internal_ArgsValuesAppender.hpp"
#include "./Internal_ArgsChecker.hpp"

#include <cassert>
#include <string>
#include <unordered_map>
#include <iostream>

namespace SimpleOverride
{
    class Internal_ReturnDataRetriever
    {
        public:
            using ReturnInfosType = std::unordered_map<std::string, Internal_OverrideReturnDataInfo>;

        protected:
            ReturnInfosType& OverrideReturnInfos;
            Internal_ArgsValuesAppender& ArgsValuesAppender;
            Internal_ArgsChecker ArgsChecker;
        
            #define SO_LOG_GetCorrectReturnDataInfo 0

            template<typename T, typename... Args>
            inline int GetCorrectReturnDataInfo(T& returnRef, std::string functionName, Args&... args)
            {
                if(OverrideReturnInfos.find(functionName) == OverrideReturnInfos.end())
                {
                    std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                    assert(false);
                    exit(1);
                }
                
                #if SO_LOG_GetCorrectReturnDataInfo
                    std::cout <<"GetCorrectReturnDataInfo called\n";
                #endif

                std::vector<void*> argumentsList;
                ArgsValuesAppender.AppendArgsValues(argumentsList, args...);
                
                std::vector<Internal_ReturnData>& curReturnDatas = 
                    OverrideReturnInfos[functionName].ReturnDatas;
                
                int returnIndex = -1;
                for(int i = 0; i < curReturnDatas.size(); i++)
                {
                    #if SO_LOG_GetCorrectReturnDataInfo
                        std::cout << "Checking return data["<<i<<"]\n";
                    #endif

                    //Check return data exist
                    if( !curReturnDatas[i].ReturnDataInfo.DataSet && 
                        !curReturnDatas[i].ReturnDataInfo.DataActionSet)
                    {
                        #if SO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at return data exist\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);
                        continue;
                    }
                
                    //Check return type
                    if(curReturnDatas[i].ReturnDataInfo.DataType != 
                        typeid(INTERNAL_SO_PURE_T).hash_code())
                    {
                        #if SO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at return type\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);
                        
                        continue;
                    }

                    //Check condition
                    if( curReturnDatas[i].ReturnConditionInfo.DataConditionSet && 
                        !curReturnDatas[i].ReturnConditionInfo.DataCondition(argumentsList))
                    {
                        #if SO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at Check condition\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);
                        
                        continue;
                    }

                    //Check parameter
                    if( !curReturnDatas[i].ReturnConditionInfo.ArgsCondition.empty() && 
                        !ArgsChecker.CheckArguments(curReturnDatas[i]   .ReturnConditionInfo
                                                                        .ArgsCondition, 
                                                    0, 
                                                    args...))
                    {
                        #if SO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at Check parameter\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);

                        continue;
                    }
                        
                    //Check times
                    if( curReturnDatas[i].ReturnConditionInfo.Times >= 0 && 
                        curReturnDatas[i].ReturnConditionInfo.CalledTimes >= 
                            curReturnDatas[i].ReturnConditionInfo.Times)
                    {
                        #if SO_LOG_GetCorrectReturnDataInfo
                            std::cout << "Failed at Check times\n";
                        #endif
                        if(curReturnDatas[i].ReturnActionInfo.OtherwiseActionSet)
                            curReturnDatas[i].ReturnActionInfo.OtherwiseAction(argumentsList);

                        continue;
                    }
                    
                    #if SO_LOG_GetCorrectReturnDataInfo
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
        public:
            inline Internal_ReturnDataRetriever(ReturnInfosType& overrideReturnInfos,
                                                Internal_ArgsValuesAppender& argsValuesAppender,
                                                Internal_ArgsChecker& argsChecker) : 
                OverrideReturnInfos(overrideReturnInfos),
                ArgsValuesAppender(argsValuesAppender),
                ArgsChecker(argsChecker)
            {}
    };
}

#endif