#ifndef SO_INTERNAL_ARGS_DATA_RETRIEVER_HPP
#define SO_INTERNAL_ARGS_DATA_RETRIEVER_HPP

#include "./Internal_ArgsTypeInfoAppender.hpp"
#include "./Internal_ArgsValuesAppender.hpp"
#include "./Internal_OverrideArgumentInfo.hpp"
#include "./Internal_ArgsChecker.hpp"

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
            Internal_ArgsChecker& ArgsChecker;
            
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
                
                std::vector<Internal_ArgumentsData>& curArgData = 
                    OverrideArgumentsInfos[functionName].ArgumentsDatas;
                
                int returnIndex = -1;
                for(int i = 0; i < curArgData.size(); i++)
                {
                    #if SO_LOG_GetCorrectArgumentsDataInfo
                        std::cout << "Checking arg data["<<i<<"]\n";
                    #endif
                
                    //Check set argument data exist
                    if(curArgData[i].ArgumentsDataInfo.size() != deRefArgumentsList.size())
                    {
                        #if SO_LOG_GetCorrectArgumentsDataInfo
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
                            curArgData[i].ArgumentsDataInfo[j].DataType != 
                                deRefArgumentsList[j].ArgTypeHash)
                        {
                            #if SO_LOG_GetCorrectArgumentsDataInfo
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
                    if( curArgData[i].ArgumentsConditionInfo.DataConditionSet && 
                        !curArgData[i].ArgumentsConditionInfo.DataCondition(argumentsList))
                    {
                        #if SO_LOG_GetCorrectArgumentsDataInfo
                            std::cout << "Failed at Check condition\n";
                        #endif
                        continue;
                    }

                    //Check parameter
                    if( !curArgData[i].ArgumentsConditionInfo.ArgsCondition.empty() && 
                        !ArgsChecker.CheckArguments(curArgData[i]   .ArgumentsConditionInfo
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
                
                //Deallocating argumentsList
                //for(int i = 0; i < argumentsList.size(); i++)
                //    argumentsList[i].Destructor(argumentsList[i].ArgData);
                
                //Deallocating derefArgumentsList
                //for(int i = 0; i < derefArgumentsList.size(); i++)
                //    derefArgumentsList[i].Destructor(derefArgumentsList[i].ArgData);

                return returnIndex;
            }
        
        public:
            Internal_ArgsDataRetriever( ArgumentInfosType& overrideArgumentsInfos,
                                        Internal_ArgsValuesAppender& argsValuesAppender,
                                        Internal_ArgsTypeInfoAppender& argsTypeInfoAppender,
                                        Internal_ArgsChecker& argsChecker) :
                                                OverrideArgumentsInfos(overrideArgumentsInfos),
                                                ArgsValuesAppender(argsValuesAppender),
                                                ArgsTypeInfoAppender(argsTypeInfoAppender),
                                                ArgsChecker(argsChecker)
            {}
        
    };

}

#endif