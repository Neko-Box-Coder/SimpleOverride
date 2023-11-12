#ifndef SO_INTERNAL_ARGS_TYPE_INFO_APPENDER_HPP
#define SO_INTERNAL_ARGS_TYPE_INFO_APPENDER_HPP

#include "./Any.hpp"
#include "./ArgsInfo.hpp"
#include "./NonComparable.hpp"
#include "./NonComparableCopyable.hpp"
#include "./NonCopyable.hpp"
#include "./PureType.hpp"
#include <vector>
namespace SimpleOverride
{
    class Internal_ArgsTypeInfoAppender
    {
        friend class Internal_ArgsDataRetriever;
        
        protected:
            inline void AppendArgsPureTypeInfo(std::vector<ArgInfo>& argumentsList) {}

            template<typename T, typename... Args>
            inline void AppendArgsPureTypeInfo( std::vector<ArgInfo>& argumentsList, 
                                                T& arg, 
                                                Args&... args)
            {
                ArgInfo curArgInfo;
                if(!std::is_same<T, Any>())
                {
                    curArgInfo.ArgSize = sizeof(INTERNAL_SO_PURE_T);
                    curArgInfo.ArgTypeHash = typeid(INTERNAL_SO_PURE_T).hash_code();
                    curArgInfo.ArgSet = true;
                }

                argumentsList.push_back(curArgInfo);
                AppendArgsPureTypeInfo(argumentsList, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendArgsPureTypeInfo( std::vector<ArgInfo>& argumentsList, 
                                                NonCopyable<T>& arg, 
                                                Args&... args)
            {
                AppendArgsPureTypeInfo(argumentsList, (INTERNAL_SO_PURE_T&)arg, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendArgsPureTypeInfo( std::vector<ArgInfo>& argumentsList, 
                                                NonComparable<T>& arg, 
                                                Args&... args)
            {
                AppendArgsPureTypeInfo(argumentsList, (INTERNAL_SO_PURE_T&)arg, args...);
            }
            
            template<typename T, typename... Args>
            inline void AppendArgsPureTypeInfo( std::vector<ArgInfo>& argumentsList, 
                                                NonComparableCopyable<T>& arg, 
                                                Args&... args)
            {
                AppendArgsPureTypeInfo(argumentsList, (INTERNAL_SO_PURE_T&)arg, args...);
            }
            
            template<   typename T, 
                        typename = typename std::enable_if<!std::is_same<T, void>::value>::type, 
                        typename = typename std::enable_if<!std::is_same<T, const void>::value>::type, 
                        typename... Args>
            inline void AppendArgsPureTypeInfo( std::vector<ArgInfo>& argumentsList, 
                                                T* arg, 
                                                Args&... args)
            {
                AppendArgsPureTypeInfo(argumentsList, *arg, args...);
            }
    };
}

#endif