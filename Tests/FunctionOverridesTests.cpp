#include "FunctionOverrides.hpp"
#include "ssTest.hpp"

namespace {
    SimpleOverride::FunctionOverrides OverrideObj;
}


int TestFuncWithoutArgs()
{
    FO_RETURN_IF_FOUND_WITHOUT_ARGS(OverrideObj, TestFuncWithoutArgs, int);
    return -1;
}

int TestFuncWithArgs(int testArg, bool testArg2, float testArg3)
{
    FO_RETURN_IF_FOUND(OverrideObj, TestFuncWithArgs, int, testArg, testArg2, testArg3);
    return -1;
}

void TestFuncWithArgsToSet(int testArg, float* testArg2, std::string& testArg3)
{
    FO_ARGUMENTS_IF_FOUND(OverrideObj, TestFuncWithArgsToSet, testArg, testArg2, testArg3);
}

template<typename T>
class ComplexArg
{
    public:
        std::string TestString = "";
        int TestInt = 0;
        float TestFloat = 0.f;
        T TestGenericType;
        
        inline ComplexArg(std::string testString, int testInt, float testFloat, T testGeneric) :    TestString(testString),
                                                                                                    TestInt(testInt),
                                                                                                    TestFloat(testFloat),
                                                                                                    TestGenericType(testGeneric)
        {}
        
        inline bool operator== (ComplexArg& other)
        {
            return TestString == other.TestString && TestInt == other.TestInt && TestFloat == other.TestFloat && TestGenericType == other.TestGenericType;
        }
        
        inline bool operator!= (ComplexArg& other)
        {
            return !ComplexArg::operator==(other);
        }
};

class ComplexClass
{
    public:
        FO_DECLARE_INSTNACE(CurrentOverrideObj);
        FO_DECLARE_OVERRIDE_METHODS(CurrentOverrideObj);
    
        inline int ComplexMemberFunction(ComplexArg<char*> charComplex, ComplexArg<std::vector<int>> vectorComplex)
        {
            FO_RETURN_IF_FOUND(CurrentOverrideObj, &ComplexClass::ComplexMemberFunction, int, charComplex, vectorComplex);
            return -1;
        }
};


int main()
{
    ssTEST_INIT();
    
    ssTEST_SET_UP
    {
        OverrideObj = SimpleOverride::FunctionOverrides();
    }
    
    ssTEST("Returns Test")
    {
        OverrideObj .OverrideReturns(TestFuncWithoutArgs)
                    .Returns(5);
        
        ssTEST_OUTPUT_ASSERT(TestFuncWithoutArgs() == 5);
        
        ssTEST_CALL_SET_UP();
        
        OverrideObj .OverrideReturns(TestFuncWithoutArgs)
                    .ReturnsByAction<int>
                    (
                        [](std::vector<SimpleOverride::ArgInfo>& args, void* out)
                        {
                            *(static_cast<int*>(out)) = 5;
                        }
                    );
        
        ssTEST_OUTPUT_ASSERT("Action", TestFuncWithoutArgs() == 5);

        ssTEST_CALL_SET_UP();
        
        OverrideObj .OverrideReturns(TestFuncWithoutArgs)
                    .Times(3);

        ssTEST_OUTPUT_ASSERT("Missing Returns", TestFuncWithoutArgs() == -1);
    }
    
    ssTEST("Arguments Test")
    {
        float setFloat = 2.f;
        std::string setString = "setString";

        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .SetArgs(SimpleOverride::ANY, setFloat, setString);

        float testFloat = 1.f;
        std::string testString = "";

        TestFuncWithArgsToSet(3, &testFloat, testString);
        
        ssTEST_OUTPUT_ASSERT(testFloat == setFloat && testString == setString);
        
        ssTEST_CALL_SET_UP();
        testFloat = 1.f;
        testString = "";
        
        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .SetArgs(SimpleOverride::ANY, setFloat, SimpleOverride::ANY);
        
        TestFuncWithArgsToSet(3, &testFloat, testString);
        ssTEST_OUTPUT_ASSERT("Any keyword", testFloat == setFloat && testString != setString);
        
        ssTEST_CALL_SET_UP();
        testFloat = 1.f;
        testString = "";
        
        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .SetArgs(SimpleOverride::ANY)
                    .SetArgsByAction<float>
                    (
                        [setFloat](std::vector<SimpleOverride::ArgInfo>& args, void* out)
                        {
                            *(static_cast<float*>(out)) = setFloat;
                        }
                    )
                    .SetArgsByAction<std::string>
                    (
                        [setString](std::vector<SimpleOverride::ArgInfo>& args, void* out)
                        {
                            *(static_cast<std::string*>(out)) = setString;
                        }
                    );
        
        TestFuncWithArgsToSet(3, &testFloat, testString);

        ssTEST_OUTPUT_ASSERT("Action", testFloat == setFloat && testString == setString);
        
        ssTEST_CALL_SET_UP();
        testFloat = 1.f;
        testString = "";
        
        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .Times(3);

        ssTEST_OUTPUT_ASSERT("Missing Arguments", testFloat == 1.f && testString == "");
    }
    
    ssTEST("Times Test")
    {
        OverrideObj .OverrideReturns(TestFuncWithoutArgs)
                    .Returns(5)
                    .Times(2);
        
        ssTEST_OUTPUT_ASSERT("Return", TestFuncWithoutArgs() == 5 && TestFuncWithoutArgs() == 5 && TestFuncWithoutArgs() == -1);
        
        OverrideObj .OverrideReturns(TestFuncWithoutArgs)
                    .Returns(3)
                    .Times(1);
        
        OverrideObj .OverrideReturns(TestFuncWithoutArgs)
                    .Returns(2)
                    .Times(1);
        
        ssTEST_OUTPUT_ASSERT("Multiple",    TestFuncWithoutArgs() == 3 && 
                                            TestFuncWithoutArgs() == 2 && 
                                            TestFuncWithoutArgs() == -1);

        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .SetArgs(SimpleOverride::ANY, 3.f, std::string("Test"))
                    .Times(2);
        
        float testFloats[3] = {1.f, 1.f, 1.f};
        std::string testStrings[3] = {"", "", ""};
        
        TestFuncWithArgsToSet(2, &testFloats[0], testStrings[0]);
        TestFuncWithArgsToSet(2, &testFloats[1], testStrings[1]);
        TestFuncWithArgsToSet(2, &testFloats[2], testStrings[2]);
        
        //for(int i = 0; i < 3; i++)
        //{
        //    std::cout << "testFloats[" << i << "]: " << testFloats[i] << std::endl;
        //    std::cout << "testStrings[" << i << "]: " << testStrings[i] << std::endl;
        //}
        
        ssTEST_OUTPUT_ASSERT("SetArgs", testFloats[0] == 3.f && testFloats[1] == 3.f && testFloats[2] == 1.f &&
                                        testStrings[0] == "Test" && testStrings[1] == "Test" && testStrings[2] == "");
    }

    ssTEST("WhenCalledWith Test")
    {
        OverrideObj .OverrideReturns(TestFuncWithArgs)
                    .Returns(5)
                    .WhenCalledWith(1, true, 3.f);
        
        ssTEST_OUTPUT_ASSERT("Return", TestFuncWithArgs(1, true, 3.f) == 5);
        
        ssTEST_OUTPUT_ASSERT("Non expected args", TestFuncWithArgs(1, true, 4.f) == -1);
    
        OverrideObj .OverrideReturns(TestFuncWithArgs)
                    .Returns(2)
                    .WhenCalledWith(2, true, 3.f);
        
        OverrideObj .OverrideReturns(TestFuncWithArgs)
                    .Returns(3)
                    .WhenCalledWith(3, false, 3.f);

        //int result = TestFuncWithArgs(2, true, 3.f);
        //int result2 = TestFuncWithArgs(1, false, 3.f);
        //std::cout << "TestFuncWithArgs(2, true, 3.f): " << result << std::endl;
        //std::cout << "TestFuncWithArgs(1, false, 3.f): " << result2 << std::endl;

        ssTEST_OUTPUT_ASSERT("Multiple",    TestFuncWithArgs(2, true, 3.f) == 2 && 
                                            TestFuncWithArgs(3, false, 3.f) == 3);
                                            
        
        OverrideObj .OverrideReturns(TestFuncWithArgs)
                    .Returns(7)
                    .WhenCalledWith(4, SimpleOverride::ANY, SimpleOverride::ANY);
        
        ssTEST_OUTPUT_ASSERT("Any", TestFuncWithArgs(4, true, 1.f) == 7 && 
                                    TestFuncWithArgs(4, false, 4.f) == 7 &&
                                    TestFuncWithArgs(5, true, 4.f) == -1);
        
        
        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .SetArgs(SimpleOverride::ANY, 3.f, std::string("Test"))
                    .WhenCalledWith(1, 2.f, std::string("Called"));
        
        float testFloat = 2.f;
        std::string testString = "Called";
        TestFuncWithArgsToSet(1, &testFloat, testString);
        
        ssTEST_OUTPUT_ASSERT("SetArgs", testFloat == 3.f && testString == "Test");
    }
    
    ssTEST("If Test")
    {
        OverrideObj .OverrideReturns(TestFuncWithArgs)
                    .Returns(5)
                    .If
                    (
                        [](std::vector<SimpleOverride::ArgInfo>& args) -> bool
                        {
                            return  args.size() == 3 && 
                                    *static_cast<int*>(args[0].ArgData) == 1 &&
                                    *static_cast<bool*>(args[1].ArgData) &&
                                    *static_cast<float*>(args[2].ArgData) == 3.f;
                        }
                    );
        
        ssTEST_OUTPUT_ASSERT("Return", TestFuncWithArgs(1, true, 3.f) == 5);
        ssTEST_OUTPUT_ASSERT("Non expected args", TestFuncWithArgs(2, true, 3.0f) == -1);
        
        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .SetArgs(SimpleOverride::ANY, 3.f, SimpleOverride::ANY)
                    .If
                    (
                        [](std::vector<SimpleOverride::ArgInfo>& args) -> bool
                        {
                            return  args.size() == 3 &&
                                    *static_cast<int*>(args[0].ArgData) == 1 &&
                                    **static_cast<float**>(args[1].ArgData) == 2.f &&
                                    *static_cast<std::string*>(args[2].ArgData) == "Test";
                        }
                    );
        
        float testFloat = 2.f;
        std::string testString = "Test";
        TestFuncWithArgsToSet(1, &testFloat, testString);
        
        ssTEST_OUTPUT_ASSERT("SetArgs", testFloat == 3.f && testString == "Test");
    }
    
    ssTEST("WhenCalledExpectedly_Do Test")
    {
        bool calledCorrectly = false;
    
        OverrideObj .OverrideReturns(TestFuncWithArgs)
                    .Returns(5)
                    .WhenCalledWith(1, true, 3.f)
                    .WhenCalledExpectedly_Do
                    (
                        [&calledCorrectly](std::vector<SimpleOverride::ArgInfo>& args)
                        {
                            calledCorrectly =   args.size() == 3 && 
                                                *static_cast<int*>(args[0].ArgData) == 1 &&
                                                *static_cast<bool*>(args[1].ArgData) &&
                                                *static_cast<float*>(args[2].ArgData);
                        }
                    );
        
        TestFuncWithArgs(2, true, 3.0f);
        ssTEST_OUTPUT_ASSERT("Non expected args", !calledCorrectly);
        
        TestFuncWithArgs(1, true, 3.f);
        ssTEST_OUTPUT_ASSERT(calledCorrectly);
        
        
        calledCorrectly = false;
        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .SetArgs(SimpleOverride::ANY, SimpleOverride::ANY, SimpleOverride::ANY)
                    .WhenCalledWith(1, 2.f, std::string("Test"))
                    .WhenCalledExpectedly_Do
                    (
                        [&calledCorrectly](std::vector<SimpleOverride::ArgInfo>& args)
                        {
                            calledCorrectly =   args.size() == 3 &&
                                                *static_cast<int*>(args[0].ArgData) == 1 &&
                                                **static_cast<float**>(args[1].ArgData) == 2.f &&
                                                *static_cast<std::string*>(args[2].ArgData) == "Test";
                        }
                    );
        
        float testFloat = 2.f;
        std::string testString = "Test";
        TestFuncWithArgsToSet(1, &testFloat, testString);
        
        ssTEST_OUTPUT_ASSERT("SetArgs", calledCorrectly);
    }
    
    ssTEST("Otherwise_Do Test")
    {
        bool calledIncorrectly = false;
    
        OverrideObj .OverrideReturns(TestFuncWithArgs)
                    .Returns(5)
                    .WhenCalledWith(1, true, 3.f)
                    .Otherwise_Do
                    (
                        [&calledIncorrectly](std::vector<SimpleOverride::ArgInfo>& args)
                        {
                            calledIncorrectly = !(args.size() == 3 && 
                                                *static_cast<int*>(args[0].ArgData) == 1 &&
                                                *static_cast<bool*>(args[1].ArgData) &&
                                                *static_cast<float*>(args[2].ArgData));
                        }
                    );
        
        TestFuncWithArgs(1, true, 3.f);
        ssTEST_OUTPUT_ASSERT("Return", !calledIncorrectly);
        
        TestFuncWithArgs(2, true, 3.f);
        ssTEST_OUTPUT_ASSERT("Non expected args", calledIncorrectly);
        
        calledIncorrectly = false;
        
        OverrideObj .OverrideArgs(TestFuncWithArgsToSet)
                    .SetArgs(SimpleOverride::ANY, SimpleOverride::ANY, std::string("What?"))
                    .WhenCalledWith(1, 2.f, std::string("Test"))
                    .Otherwise_Do
                    (
                        [&calledIncorrectly](std::vector<SimpleOverride::ArgInfo>& args)
                        {
                            calledIncorrectly = args.size() == 3 &&
                                                *static_cast<int*>(args[0].ArgData) == 1 &&
                                                **static_cast<float**>(args[1].ArgData) == 2.f &&
                                                *static_cast<std::string*>(args[2].ArgData) == "Test2";
                        }
                    );
        
        float testFloat = 2.f;
        std::string testString = "Test2";
        TestFuncWithArgsToSet(1, &testFloat, testString);
        ssTEST_OUTPUT_ASSERT("SetArgs", calledIncorrectly && testString == "Test2");
    }
    
    ssTEST("Complex Test")
    {
        ComplexClass testClass;
        
        char testChar[] = "Test";
        std::vector<int> testIntVec = {1, 2, 3, 4};
        
        ComplexArg<char*> testCharComplex = ComplexArg<char*>("test", 1, 2.f, testChar);
        ComplexArg<std::vector<int>> testIntComplex = ComplexArg<std::vector<int>>("test2", 2, 3.f, testIntVec);
    
        bool callAllow = true;
        
        testClass   .OverrideReturns(&ComplexClass::ComplexMemberFunction)
                    .Returns(5)
                    .WhenCalledWith(testCharComplex, testIntComplex)
                    .If([&callAllow](std::vector<SimpleOverride::ArgInfo>&){return callAllow;});
        
        ComplexArg<char*> callCharComplex = testCharComplex;
        ComplexArg<std::vector<int>> callIntComplex = testIntComplex;
        
        ssTEST_OUTPUT_ASSERT("Return",  testClass.ComplexMemberFunction(callCharComplex, callIntComplex) == 5);
        
        callAllow = false;
        
        ssTEST_OUTPUT_ASSERT("Return Disallowed",  testClass.ComplexMemberFunction(callCharComplex, callIntComplex) == -1);
        
        callAllow = true;
        callIntComplex.TestGenericType[2] = 4;
        ssTEST_OUTPUT_ASSERT("Return when Not equal",  testClass.ComplexMemberFunction(callCharComplex, callIntComplex) == -1);
        
        testClass   .OverrideReturns(&ComplexClass::ComplexMemberFunction)
                    .ReturnsByAction<int>
                    (
                        [](std::vector<SimpleOverride::ArgInfo>& args, void* out)
                        {
                            (*static_cast<int*>(out)) = 6;
                        }
                    )
                    .WhenCalledWith(SimpleOverride::ANY, SimpleOverride::ANY);
        
        ssTEST_OUTPUT_ASSERT("Return with Action",  testClass.ComplexMemberFunction(callCharComplex, callIntComplex) == 6);
        
    }

    ssTEST_END();
    
    return 0;
}