#include "SimpleOverride.hpp"
#include "ssTest.hpp"
#include "./FileFunctions.hpp"
#include "./ClassFunctions.hpp"

SimpleOverride::Overrider OverrideObj;

int main()
{
    ssTEST_INIT();
    ssTEST_SET_UP
    {
        OverrideObj = SimpleOverride::Overrider();
    };
    
    //TODO:
    //- Test primitive types
    //- Test objects
    //- Test string
    //- Test template objects
    
    ssTEST("Primitive types Test")
    {
        Rectangle rect(1.5, 1.5);
        
        SO_OVERRIDE_RETURNS (OverrideObj, TestFuncWithoutArgs())
                            .Returns(32);

        SO_OVERRIDE_RETURNS (rect, GetWidth())
                            .Returns(5.f);
        
        ssTEST_OUTPUT_ASSERT("int", FuncWithoutArgs() == 32);
    
        ssTEST_OUTPUT_ASSERT("float", rect.GetWidth() == 5.f);
    };
    
    ssTEST("Object Test")
    {
        DummyClass assertObject(1, 2.0, "test");
        
        SO_OVERRIDE_RETURNS (OverrideObj, ReturnObjectFunc(int, double, std::string))
                            .Returns(assertObject);
        
        ssTEST_OUTPUT_ASSERT(ReturnObjectFunc(1, 3.0, "test 2") == assertObject);
    };
    
    ssTEST("String Test")
    {
        SO_OVERRIDE_RETURNS (OverrideObj, ReturnStringFunc(int))
                            .Returns("test");
        
        ssTEST_OUTPUT_ASSERT(ReturnStringFunc(1) == "test");
    };
    
    ssTEST("Template Object Test")
    {
        
        
        
        SO_OVERRIDE_RETURNS (OverrideObj, ReturnTemplatedObjectFunc(std::string, int, float, int))
                            .Returns(assertObject);
        
        ssTEST_OUTPUT_ASSERT(   ReturnTemplatedObjectFunc( "test string 2", 
                                                            3, 
                                                            5.0f, 
                                                            3) == assertObject);
    }
    
    
    ssTEST_END();
    return 0;
}
