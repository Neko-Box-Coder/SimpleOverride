#ifndef SO_INTERNAL_ARGS_VALUES_CHECKER_HPP
#define SO_INTERNAL_ARGS_VALUES_CHECKER_HPP

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
    class Internal_ArgsValuesChecker
    {
        friend class Internal_ReturnDataRetriever;
        friend class Internal_ArgsDataRetriever;
        
        protected:
            inline bool CheckArgumentsValues(   std::vector<ArgInfo>& validArgumentsList, 
                                                int argIndex) { return true; };

            #define SO_LOG_CheckArgumentsValues 0

            template<typename T, typename... Args>
            inline bool CheckArgumentsValues(   std::vector<ArgInfo>& validArgumentsList, 
                                                int argIndex, 
                                                T& arg, 
                                                Args&... args)
            {
                #if SO_LOG_CheckArgumentsValues
                    std::cout <<"CheckArgumentsValues index: "<<argIndex<<"\n";
                #endif
            
                if(argIndex >= validArgumentsList.size())
                    return false;

                if(validArgumentsList[argIndex].ArgSet)
                {
                    if(arg != *reinterpret_cast<INTERNAL_SO_PURE_T*>
                                (validArgumentsList[argIndex].ArgData))
                    {
                        #if SO_LOG_CheckArgumentsValues
                            std::cout << 
                            "arg != *reinterpret_cast<INTERNAL_SO_PURE_T*>\
                            (validArgumentsList[argIndex].ArgData\n";
                        
                        #endif
                        return false;
                    }
                }
                
                #if SO_LOG_CheckArgumentsValues
                    std::cout <<"CheckArgumentsValues index: "<<argIndex<<" passed\n";
                #endif
                
                return CheckArgumentsValues(validArgumentsList, ++argIndex, args...);
            }
            
            template<typename... Args>
            inline bool CheckArgumentsValues(   std::vector<ArgInfo>& validArgumentsList, 
                                                int argIndex, 
                                                Any& arg, 
                                                Args&... args)
            {
                #if SO_LOG_CheckArgumentsValues
                    std::cout <<"CheckArgumentsValues index: "<<argIndex<<"\n";
                #endif
            
                if(argIndex >= validArgumentsList.size())
                    return false;

                if(validArgumentsList[argIndex].ArgSet)
                    return false;
                
                #if SO_LOG_CheckArgumentsValues
                    std::cout <<"CheckArgumentsValues index: "<<argIndex<<" passed\n";
                #endif
                
                return CheckArgumentsValues(validArgumentsList, ++argIndex, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArgumentsValues(   std::vector<ArgInfo>& validArgumentsList, 
                                                int argIndex, 
                                                NonCopyable<T>& arg, 
                                                Args&... args)
            {
                return CheckArgumentsValues(validArgumentsList, argIndex, (T&)arg, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArgumentsValues(   std::vector<ArgInfo>& validArgumentsList, 
                                                int argIndex, 
                                                NonComparable<T>& arg, 
                                                Args&... args)
            {
                if(argIndex >= validArgumentsList.size())
                    return false;
            
                return  !validArgumentsList[argIndex].ArgSet ? 
                        CheckArgumentsValues(validArgumentsList, ++argIndex, args...) : 
                        false;
            }
            
            template<typename T, typename... Args>
            inline bool CheckArgumentsValues(   std::vector<ArgInfo>& validArgumentsList, 
                                                int argIndex, 
                                                NonComparableCopyable<T>& arg, 
                                                Args&... args)
            {
                if(argIndex >= validArgumentsList.size())
                    return false;

                return  !validArgumentsList[argIndex].ArgSet ? 
                        CheckArgumentsValues(validArgumentsList, ++argIndex, args...) : 
                        false;
            }
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline bool CheckArgumentsValues(   std::vector<ArgInfo>& validArgumentsList, 
                                                int argIndex, 
                                                T*& arg, 
                                                Args&... args)
            {
                return CheckArgumentsValues(validArgumentsList, argIndex, *arg, args...);
            }
            
            template<typename T, typename... Args>
            inline bool CheckArgumentsValues(   std::vector<ArgInfo>& validArgumentsList, 
                                                int argIndex, 
                                                const T& arg, 
                                                Args&... args)
            {
                return CheckArgumentsValues(validArgumentsList, 
                                            argIndex, 
                                            const_cast<INTERNAL_SO_PURE_T&>(arg), 
                                            args...);
            }
    };
}

#endif