#ifndef SO_INTERNAL_ARGS_DATA_RETRIEVER_HPP
#define SO_INTERNAL_ARGS_DATA_RETRIEVER_HPP

#include "./Internal_ArgsTypeInfoAppender.hpp"
#include "./Internal_ArgsValuesAppender.hpp"
#include "./Internal_OverrideArgumentInfo.hpp"
#include "./Internal_ArgsTypesChecker.hpp"
#include "./Internal_ArgsValuesChecker.hpp"

#include <cassert>
#include <string>
#include <unordered_map>
#include <iostream>

namespace SimpleOverride
{
    class Internal_ArgsDataRetriever
    {
        public:
            using ArgumentInfosType = std::unordered_map<std::string, Internal_OverrideArgumentInfo>;
        
        protected:
            ArgumentInfosType& OverrideArgumentsInfos;
            Internal_ArgsValuesAppender& ArgsValuesAppender;
            Internal_ArgsTypeInfoAppender& ArgsTypeInfoAppender;
            Internal_ArgsTypesChecker& ArgsTypesChecker;
            Internal_ArgsValuesChecker& ArgsValuesChecker;
            
            #define SO_LOG_GetCorrectArgumentsDataInfo 0

            template<typename... Args>
            inline int GetCorrectArgumentsDataInfo(std::string functionName, Args&... args)
            {
                if(OverrideArgumentsInfos.find(functionName) == OverrideArgumentsInfos.end())
                {
                    std::cout << "[ERROR] This should be checked before calling this" << std::endl;
                    assert(false);
                    exit(1);
                }
                
                #if SO_LOG_GetCorrectArgumentsDataInfo
                    std::cout <<"GetCorrectArgumentsDataInfo called\n";
                #endif
                
                std::vector<void*> argumentsList;
                ArgsValuesAppender.AppendArgsValues(argumentsList, args...);
                
                std::vector<ArgInfo> deRefArgumentsList;
                ArgsTypeInfoAppender.AppendArgsPureTypeInfo(deRefArgumentsList, args...);
                
                std::vector<Internal_ArgsData>& curArgData = 
                    OverrideArgumentsInfos[functionName].ArgumentsDatas;
                
                int returnIndex = -1;
                for(int i = 0; i < curArgData.size(); i++)
                {
                    #if SO_LOG_GetCorrectArgumentsDataInfo
                        std::cout << "Checking arg data["<<i<<"]\n";
                    #endif
                    
                    //Check set argument data counts match
                    if(curArgData[i].ArgumentsDataInfo.size() != deRefArgumentsList.size())
                    {
                        #if SO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check set argument data exist\n";
                        #endif
                        continue;
                    }
                    
                    //Check override argument data types match
                    bool argumentTypeFailed = false;
                    for(int j = 0; j < curArgData[i].ArgumentsDataInfo.size(); j++)
                    {
                        bool overrideArg =  curArgData[i].ArgumentsDataInfo[j].DataSet || 
                                            curArgData[i].ArgumentsDataInfo[j].DataActionSet;
                        
                        if( overrideArg && 
                            curArgData[i].ArgumentsDataInfo[j].DataType != 
                                deRefArgumentsList[j].ArgTypeHash)
                        {
                            #if SO_LOG_GetCorrectArgumentsDataInfo
                                std::cout << "Failed at Check arguments types\n";
                            #endif
                            argumentTypeFailed = true;
                            break;
                        }
                    }
                    if(argumentTypeFailed)
                        continue;
                    
                    //Check parameter condition types/count match
                    if( !curArgData[i].ArgumentsConditionInfo.ArgsCondition.empty() && 
                        !ArgsTypesChecker.CheckArgumentsTypes(  curArgData[i]   .ArgumentsConditionInfo
                                                                                .ArgsCondition, 
                                                                0, 
                                                                args...))
                    {
                        #if SO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check parameter\n";
                        #endif
                        continue;
                    }
                    
                    //Check parameter values
                    if( !curArgData[i].ArgumentsConditionInfo.ArgsCondition.empty() && 
                        !ArgsValuesChecker.CheckArgumentsValues(curArgData[i]   .ArgumentsConditionInfo
                                                                                .ArgsCondition, 
                                                                0, 
                                                                args...))
                    {
                        #if SO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check parameter\n";
                        #endif
                        
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.OtherwiseAction(argumentsList);
                        
                        continue;
                    }
                    
                    //Check condition lambda
                    if( curArgData[i].ArgumentsConditionInfo.DataConditionSet && 
                        !curArgData[i].ArgumentsConditionInfo.DataCondition(argumentsList))
                    {
                        #if SO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check condition\n";
                        #endif
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.OtherwiseAction(argumentsList);
                        
                        continue;
                    }
                    
                    //Check times
                    if( curArgData[i].ArgumentsConditionInfo.Times >= 0 && 
                        curArgData[i].ArgumentsConditionInfo.CalledTimes >= 
                            curArgData[i].ArgumentsConditionInfo.Times)
                    {
                        #if SO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check times\n";
                        #endif
                        if(curArgData[i].ArgumentsActionInfo.OtherwiseActionSet)
                            curArgData[i].ArgumentsActionInfo.OtherwiseAction(argumentsList);
                        continue;
                    }
                    
                    #if SO_LOG_GetCorrectArgumentsDataInfo
                        std::cout << "Argument data found: "<<i<<"\n";
                    #endif
                    
                    returnIndex = i;
                    break;
                }
                
                //NOTE: We don't need to deallocate argumentsList and derefArgumentsList 
                //  because they are just pointers to arg values and type info from the caller
                return returnIndex;
            }
        
        public:
            Internal_ArgsDataRetriever( ArgumentInfosType& overrideArgumentsInfos,
                                        Internal_ArgsValuesAppender& argsValuesAppender,
                                        Internal_ArgsTypeInfoAppender& argsTypeInfoAppender,
                                        Internal_ArgsTypesChecker& argsTypesChecker,
                                        Internal_ArgsValuesChecker& argsValuesChecker) : 
                                                OverrideArgumentsInfos(overrideArgumentsInfos),
                                                ArgsValuesAppender(argsValuesAppender),
                                                ArgsTypeInfoAppender(argsTypeInfoAppender),
                                                ArgsTypesChecker(argsTypesChecker),
                                                ArgsValuesChecker(argsValuesChecker)
            {}
    };

}

#endif