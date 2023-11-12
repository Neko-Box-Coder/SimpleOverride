#ifndef SO_INTERNAL_ARGS_CHECKER_HPP
#define SO_INTERNAL_ARGS_CHECKER_HPP

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
    class Internal_ArgsChecker
    {
        friend class Internal_ReturnDataRetriever;
        friend class Internal_ArgsDataRetriever;
        
        protected:
            inline bool CheckArguments( std::vector<ArgInfo>& argumentsListToCheck, 
                                        int argIndex) { return true; };

            #define SO_LOG_CheckArguments 0

            template<typename T, typename... Args>
            inline bool CheckArguments( std::vector<ArgInfo>& argumentsListToCheck, 
                                        int argIndex, 
                                        T& arg, 
                                        Args&... args)
            {
                #if SO_LOG_CheckArguments
                    std::cout <<"CheckArguments index: "<<argIndex<<"\n";
                #endif
            
                if(argIndex >= argumentsListToCheck.size())
                    return false;

                if(argumentsListToCheck[argIndex].ArgSet)
                {
                    if(sizeof(INTERNAL_SO_PURE_T) != argumentsListToCheck[argIndex].ArgSize)
                    {
                        #if SO_LOG_CheckArguments
                            std::cout <<    "sizeof(INTERNAL_SO_PURE_T): " <<
                                            sizeof(INTERNAL_SO_PURE_T) <<
                                            "\n";
                            
                            std::cout << "sizeof(T): "<<sizeof(T)<<"\n";
                            std::cout <<    "argumentsListToCheck[" << 
                                            argIndex << 
                                            "].ArgSize: " << 
                                            argumentsListToCheck[argIndex].ArgSize <<
                                            "\n";
                        #endif
                        return false;
                    }

                    if(typeid(INTERNAL_SO_PURE_T).hash_code() != 
                            argumentsListToCheck[argIndex].ArgTypeHash)
                    {
                        #if SO_LOG_CheckArguments
                            std::cout <<    "typeid(INTERNAL_SO_PURE_T).hash_code(): " <<
                                            typeid(INTERNAL_SO_PURE_T).hash_code() << 
                                            "\n";
                            
                            std::cout <<    "argumentsListToCheck["
                                            <<argIndex << 
                                            "].ArgTypeHash: " <<
                                            argumentsListToCheck[argIndex].ArgTypeHash <<
                                            "\n";
                        #endif
                        return false;
                    }

                    if(arg != *reinterpret_cast<INTERNAL_SO_PURE_T*>
                                (argumentsListToCheck[argIndex].ArgData))
                    {
                        #if SO_LOG_CheckArguments
                            std::cout << 
                            "arg != *reinterpret_cast<INTERNAL_SO_PURE_T*>\
                            (argumentsListToCheck[argIndex].ArgData\n";
                        
                        #endif
                        return false;
                    }
                }            
                
                #if SO_LOG_CheckArguments
                    std::cout <<"CheckArguments index: "<<argIndex<<" passed\n";
                #endif
                
                return CheckArguments(argumentsListToCheck, ++argIndex, args...);
            }
            
            template<typename... Args>
            inline bool CheckArguments( std::vector<ArgInfo>& argumentsListToCheck, 
                                        int argIndex, 
                                        Any& arg, 
                                        Args&... args)
            {
                #if SO_LOG_CheckArguments
                    std::cout <<"CheckArguments index: "<<argIndex<<"\n";
                #endif
            
                if(argIndex >= argumentsListToCheck.size())
                    return false;

                if(argumentsListToCheck[argIndex].ArgSet)
                    return false;
                
                #if SO_LOG_CheckArguments
                    std::cout <<"CheckArguments index: "<<argIndex<<" passed\n";
                #endif
                
                return CheckArguments(argumentsListToCheck, ++argIndex, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArguments( std::vector<ArgInfo>& argumentsListToCheck, 
                                        int argIndex, 
                                        NonCopyable<T>& arg, 
                                        Args&... args)
            {
                return CheckArguments(argumentsListToCheck, argIndex, (T&)arg, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArguments( std::vector<ArgInfo>& argumentsListToCheck, 
                                        int argIndex, 
                                        NonComparable<T>& arg, 
                                        Args&... args)
            {
                if(argIndex >= argumentsListToCheck.size())
                    return false;
            
                return  !argumentsListToCheck[argIndex].ArgSet ? 
                        CheckArguments(argumentsListToCheck, ++argIndex, args...) : 
                        false;
            }
            
            template<typename T, typename... Args>
            inline bool CheckArguments( std::vector<ArgInfo>& argumentsListToCheck, 
                                        int argIndex, 
                                        NonComparableCopyable<T>& arg, 
                                        Args&... args)
            {
                if(argIndex >= argumentsListToCheck.size())
                    return false;

                return  !argumentsListToCheck[argIndex].ArgSet ? 
                        CheckArguments(argumentsListToCheck, ++argIndex, args...) : 
                        false;
            }
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline bool CheckArguments( std::vector<ArgInfo>& argumentsListToCheck, 
                                        int argIndex, 
                                        T*& arg, 
                                        Args&... args)
            {
                return CheckArguments(argumentsListToCheck, argIndex, *arg, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArguments( std::vector<ArgInfo>& argumentsListToCheck, 
                                        int argIndex, 
                                        const T& arg, 
                                        Args&... args)
            {
                return CheckArguments(  argumentsListToCheck, 
                                        argIndex, 
                                        const_cast<INTERNAL_SO_PURE_T&>(arg), 
                                        args...);
            }
    };
}

#endif