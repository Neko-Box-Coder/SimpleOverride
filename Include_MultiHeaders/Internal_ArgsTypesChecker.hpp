#ifndef SO_INTERNAL_ARGS_TYPES_CHECKER_HPP
#define SO_INTERNAL_ARGS_TYPES_CHECKER_HPP

#include "./Any.hpp"
#include "./ArgsInfo.hpp"
#include "./NonComparable.hpp"
#include "./NonComparableCopyable.hpp"
#include "./NonCopyable.hpp"
#include "./PureType.hpp"
#include <vector>
#include <iostream>

namespace SimpleOverride
{
    class Internal_ArgsTypesChecker
    {
        friend class Internal_ReturnDataRetriever;
        friend class Internal_ArgsDataRetriever;
        
        protected:
            inline bool CheckArgumentsTypes(std::vector<ArgInfo>& validArgumentsList, 
                                            int argIndex) { return true; };

            #define SO_LOG_CheckArguments 0

            template<typename T, typename... Args>
            inline bool CheckArgumentsTypes(std::vector<ArgInfo>& validArgumentsList, 
                                            int argIndex, 
                                            T& arg, 
                                            Args&... args)
            {
                #if SO_LOG_CheckArguments
                    std::cout <<"CheckArgumentsTypes index: "<<argIndex<<"\n";
                #endif
            
                if(argIndex >= validArgumentsList.size())
                    return false;

                if(validArgumentsList[argIndex].ArgSet)
                {
                    if(sizeof(INTERNAL_SO_NON_CONST_T) != validArgumentsList[argIndex].ArgSize)
                    {
                        #if SO_LOG_CheckArguments
                            std::cout <<    "sizeof(INTERNAL_SO_NON_CONST_T): " <<
                                            sizeof(INTERNAL_SO_NON_CONST_T) <<
                                            "\n";
                            
                            std::cout << "sizeof(T): "<<sizeof(T)<<"\n";
                            std::cout <<    "validArgumentsList[" << 
                                            argIndex << 
                                            "].ArgSize: " << 
                                            validArgumentsList[argIndex].ArgSize <<
                                            "\n";
                        #endif
                        return false;
                    }

                    if(typeid(INTERNAL_SO_NON_CONST_T).hash_code() != 
                            validArgumentsList[argIndex].ArgTypeHash)
                    {
                        #if SO_LOG_CheckArguments
                            std::cout <<    "typeid(INTERNAL_SO_NON_CONST_T).hash_code(): " <<
                                            typeid(INTERNAL_SO_NON_CONST_T).hash_code() << 
                                            "\n";
                            
                            std::cout <<    "validArgumentsList["
                                            <<argIndex << 
                                            "].ArgTypeHash: " <<
                                            validArgumentsList[argIndex].ArgTypeHash <<
                                            "\n";
                        #endif
                        return false;
                    }
                }
                
                #if SO_LOG_CheckArguments
                    std::cout <<"CheckArgumentsTypes index: "<<argIndex<<" passed\n";
                #endif
                
                return CheckArgumentsTypes(validArgumentsList, ++argIndex, args...);
            }
            
            template<typename... Args>
            inline bool CheckArgumentsTypes(std::vector<ArgInfo>& validArgumentsList, 
                                            int argIndex, 
                                            Any& arg, 
                                            Args&... args)
            {
                #if SO_LOG_CheckArguments
                    std::cout <<"CheckArgumentsTypes index: "<<argIndex<<"\n";
                #endif
            
                if(argIndex >= validArgumentsList.size())
                    return false;

                if(validArgumentsList[argIndex].ArgSet)
                    return false;
                
                #if SO_LOG_CheckArguments
                    std::cout <<"CheckArgumentsTypes index: "<<argIndex<<" passed\n";
                #endif
                
                return CheckArgumentsTypes(validArgumentsList, ++argIndex, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArgumentsTypes(std::vector<ArgInfo>& validArgumentsList, 
                                            int argIndex, 
                                            NonCopyable<T>& arg, 
                                            Args&... args)
            {
                return CheckArgumentsTypes(validArgumentsList, argIndex, (T&)arg, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArgumentsTypes(std::vector<ArgInfo>& validArgumentsList, 
                                            int argIndex, 
                                            NonComparable<T>& arg, 
                                            Args&... args)
            {
                if(argIndex >= validArgumentsList.size())
                    return false;
            
                return  !validArgumentsList[argIndex].ArgSet ? 
                        CheckArgumentsTypes(validArgumentsList, ++argIndex, args...) : 
                        false;
            }
            
            template<typename T, typename... Args>
            inline bool CheckArgumentsTypes(std::vector<ArgInfo>& validArgumentsList, 
                                            int argIndex, 
                                            NonComparableCopyable<T>& arg, 
                                            Args&... args)
            {
                if(argIndex >= validArgumentsList.size())
                    return false;

                return  !validArgumentsList[argIndex].ArgSet ? 
                        CheckArgumentsTypes(validArgumentsList, ++argIndex, args...) : 
                        false;
            }
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline bool CheckArgumentsTypes(std::vector<ArgInfo>& validArgumentsList, 
                                            int argIndex, 
                                            T*& arg, 
                                            Args&... args)
            {
                return CheckArgumentsTypes(validArgumentsList, argIndex, *arg, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArgumentsTypes(std::vector<ArgInfo>& validArgumentsList, 
                                            int argIndex, 
                                            const T& arg, 
                                            Args&... args)
            {
                return CheckArgumentsTypes( validArgumentsList, 
                                            argIndex, 
                                            const_cast<INTERNAL_SO_NON_CONST_T&>(arg), 
                                            args...);
            }
    };
}

#endif