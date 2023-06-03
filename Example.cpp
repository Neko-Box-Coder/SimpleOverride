#include "FunctionOverrides.hpp"

using namespace SimpleOverride;

FunctionOverrides Overrider;

int ChangeReturnValue(int a, float* b)
{
    //FO_RETURN_IF_FOUND(functionOverrideObj, functionRef, returnType, args...)
    FO_RETURN_IF_FOUND(Overrider, ChangeReturnValue(int, float*), int, a, b);
    
    return 0;
}

void SetArgumentValue(int a, float& b, int* c)
{
    //FO_ARGUMENTS_IF_FOUND(functionOverrideObj, functionRef, args...)
    FO_ARGUMENTS_IF_FOUND(Overrider, SetArgumentValue(int, float&, int*), a, b, c);
    
    //You can also use FO_ARGUMENTS_AND_RETURN_IF_FOUND to return a specify value
    //  When the condition is met
}

int main()
{
    //Example of overriding return value
    Overrider   .OverrideReturns(ChangeReturnValue)
                .Returns(1)
                .WhenCalledWith(2, 3.f)     //Pointers are automatically dereferenced when getting compared.
                                            //      Unless it is cast to void*, then it won't be dereferenced.
                .Times(2);                  //By default, it is infinite times if not specified

    float testFloat = 3.f;
    assert(ChangeReturnValue(2, &testFloat) == 1 && ChangeReturnValue(2, &testFloat) == 1);
    
    testFloat = 4.f;
    assert(ChangeReturnValue(4, &testFloat) == 0);
    
    
    //Example of overriding argument value
    Overrider   .OverrideArgs(SetArgumentValue)
                .SetArgs(FO_DONT_SET, 2.f, 3)   //Again, pointers are automatically dereferenced when getting set
                .WhenCalledWith(4, FO_ANY, FO_ANY);

    int testInt = 5;
    SetArgumentValue(4, testFloat, &testInt);
    assert(testFloat == 2.f && testInt == 3);
    
    std::cout << "Success\n";
    
    //------------------------------------------------
    //Extra:
    //------------------------------------------------
    
    //ReturnsByAction and SetArgsByAction can also be used to return or set arguments by lambda
    
    //FO_DECLARE_INSTNACE(OverrideObjName) macro can be used inside a class declaration to 
    //  declare an instance of overriding object

    //FO_DECLARE_OVERRIDE_METHODS(OverrideObjName) macro can be used inside a class declaration 
    //  to declare proxy methods inline implementations to the override object
    
    //FO_NonCopyable and FO_NonComparable can be used for objects that are not copyable or comparable.
    //  These objects won't be compared against in WhenCalledWith.
    
    //You also have access to "If" clause for additonal conditions
    
    //Similarly, you have "WhenCalledExpectedly_Do" and "Otherwise_Do" clauses for performing custom actions
    //  when the condition is matched or not respectively
    
    //For usage on these extra actions, see Tests/FunctionOverridesTests.cpp
    
    return 0;
}