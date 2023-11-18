#ifndef SO_FILE_FUNCTIONS_HPP
#define SO_FILE_FUNCTIONS_HPP

#include "./Components/ComplexClass.hpp"
#include "./Components/NonCopyableTestClass.hpp"
#include "./Components/NonComparableTestClass.hpp"
#include "./Components/DummyClass.hpp"
#include "SimpleOverride.hpp"

extern SimpleOverride::Overrider OverrideObj;

inline int FuncWithoutArgs()
{
    SO_RETURN_IF_FOUND(OverrideObj, FuncWithoutArgs(), int);
    return -1;
}

inline int FuncWithArgs(int testArg, bool testArg2, float testArg3)
{
    SO_RETURN_IF_FOUND( OverrideObj, 
                        FuncWithArgs(int, bool, float), 
                        int, 
                        testArg, 
                        testArg2, 
                        testArg3);
    return -1;
}

inline int FuncWithConstArgs(const int testArg, const bool testArg2, float testArg3)
{
    SO_RETURN_IF_FOUND( OverrideObj, 
                        FuncWithConstArgs(const int, const bool, float), 
                        int, 
                        testArg, 
                        testArg2, 
                        testArg3);
    return -1;
}

inline void* FuncWIthVoidPointer(int testArg, void* testArg2)
{
    SO_RETURN_IF_FOUND(OverrideObj, FuncWIthVoidPointer(int, void*), void*, testArg, testArg2);
    return nullptr;
}

inline void FuncWithArgsToSet(int testArg, float* testArg2, std::string& testArg3)
{
    SO_MODIFY_ARGS_IF_FOUND(   OverrideObj, 
                                    FuncWithArgsToSet(int, float*, std::string&), 
                                    testArg, 
                                    testArg2, 
                                    testArg3);
}

inline void FuncWithConstArgsAndArgsToSet(  const int testArg, 
                                            const float testArg2, 
                                            std::string& testArg3)
{
    SO_MODIFY_ARGS_IF_FOUND(   OverrideObj, 
                                    FuncWithConstArgsAndArgsToSet(  const int, 
                                                                        const float, 
                                                                        std::string&), 
                                    testArg, 
                                    testArg2, 
                                    testArg3);
}

template<typename T>
inline int TemplateFunction(T testArg)
{
    SO_RETURN_IF_FOUND( OverrideObj, 
                        template<typename T> int TemplateFunctionTest(T),
                        int,
                        testArg);

    return -1;
}

inline DummyClass ReturnObjectFunc(int data, double value, std::string name)
{
    SO_MODIFY_ARGS_IF_FOUND(   OverrideObj, 
                                    ReturnObjectFunc(int, double, std::string), 
                                    data,
                                    value,
                                    name);
    
    SO_RETURN_IF_FOUND( OverrideObj,
                        ReturnObjectFunc(int, double, std::string),
                        DummyClass,
                        data,
                        value,
                        name);

    return DummyClass(data, value, name);
}

inline bool SetObjectFunc(int data, double value, std::string name, DummyClass& dummyClass)
{
    SO_MODIFY_ARGS_AND_RETURN_IF_FOUND( OverrideObj, 
                                        true,
                                        SetObjectFunc(int, double, std::string, DummyClass&), 
                                        data, 
                                        value, 
                                        name, 
                                        dummyClass);

    SO_RETURN_IF_FOUND( OverrideObj,
                        SetObjectFunc(int, double, std::string, DummyClass&),
                        bool,
                        data,
                        value,
                        name,
                        dummyClass);
    
    dummyClass = DummyClass(data, value, name);
    return true;
}

inline std::string ReturnStringFunc(int value)
{
    SO_RETURN_IF_FOUND( OverrideObj, 
                        ReturnStringFunc(int), 
                        std::string, 
                        value);
    
    return "";
}

template<typename T>
inline T ReturnTemplateObjectFunc(T testArg)
{
    SO_RETURN_IF_FOUND( OverrideObj, 
                        ReturnTemplateObjectFunc(T), 
                        T, 
                        testArg);
    
    return testArg;
}

template<typename T>
inline void SetTemplateObjectFunc(T& testArg, T* testArg2)
{
    SO_MODIFY_ARGS_IF_FOUND(OverrideObj, 
                            SetTemplateObjectFunc(T&, T*),
                            testArg,
                            testArg2);
}

#endif