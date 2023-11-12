#ifndef SO_FILE_FUNCTIONS_TESTS_HPP
#define SO_FILE_FUNCTIONS_TESTS_HPP

#include "SimpleOverride.hpp"


int TestFuncWithoutArgs()
{
    SO_RETURN_IF_FOUND(OverrideObj, TestFuncWithoutArgs(), int);
    return -1;
}

int TestFuncWithArgs(int testArg, bool testArg2, float testArg3)
{
    SO_RETURN_IF_FOUND(OverrideObj, TestFuncWithArgs(int, bool, float), int, testArg, testArg2, testArg3);
    return -1;
}

int TestFuncWithConstArgs(const int testArg, const bool testArg2, float testArg3)
{
    SO_RETURN_IF_FOUND(OverrideObj, TestFuncWithConstArgs(const int, const bool, float), int, testArg, testArg2, testArg3);
    return -1;
}

void* TestFuncWIthVoidPointer(int testArg, void* testArg2)
{
    SO_RETURN_IF_FOUND(OverrideObj, TestFuncWIthVoidPointer(int, void*), void*, testArg, testArg2);
    return nullptr;
}

void TestFuncWithArgsToSet(int testArg, float* testArg2, std::string& testArg3)
{
    SO_MODIFY_ARGUMENTS_IF_FOUND(OverrideObj, TestFuncWithArgsToSet(int, float*, std::string&), testArg, testArg2, testArg3);
}

void TestFuncWithConstArgsAndArgsToSet(const int testArg, const float testArg2, std::string& testArg3)
{
    SO_MODIFY_ARGUMENTS_IF_FOUND(OverrideObj, TestFuncWithConstArgsAndArgsToSet(const int, const float, std::string&), testArg, testArg2, testArg3);
}

int TestFuncWithNonCopyableNonComparableArg(int testArg, ComplexClass& nonCopyableComparableArg)
{
    SO_RETURN_IF_FOUND( OverrideObj, 
                        TestFuncWithNonCopyableArg(int, ComplexClass&), 
                        int, 
                        testArg, 
                        (SO_NonComparableCopyable<ComplexClass>&)nonCopyableComparableArg);
    return -1;
}

int TestFuncWithNonCopyableArg(int testArg, NonCopyableTestClass& nonCopyableArg)
{
    SO_RETURN_IF_FOUND( OverrideObj, 
                        TestFuncWithNonCopyableArg(int, NonCopyableTestClass&), 
                        int, 
                        testArg, 
                        (SO_NonCopyable<NonCopyableTestClass>&)nonCopyableArg);
    
    return -1;
}

void TestFuncWithNonComparableArg(int testArg, NonComparableTestClass& nonComparableArg)
{
    SO_MODIFY_ARGUMENTS_IF_FOUND(   OverrideObj, 
                                    TestFuncWithNonComparableArg(int, NonComparableTestClass&), 
                                    testArg, 
                                    (SO_NonComparable<NonComparableTestClass>&)nonComparableArg);
}

template<typename T>
int TemplateFunctionTest(T testArg)
{
    SO_RETURN_IF_FOUND( OverrideObj, 
                        template<typename T>\
                        int TemplateFunctionTest(T),
                        int,
                        testArg);

    return -1;
}


#endif