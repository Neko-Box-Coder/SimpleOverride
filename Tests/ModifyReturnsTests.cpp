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

    ssTEST("Return Primitive types Test")
    {
        Rectangle rect(1.5, 1.5);
        
        SO_OVERRIDE_RETURNS (OverrideObj, TestFuncWithoutArgs())
                            .Returns(32);

        SO_OVERRIDE_RETURNS (rect, GetWidth())
                            .Returns(5.f);
        
        ssTEST_OUTPUT_ASSERT("int", FuncWithoutArgs() == 32);
    
        ssTEST_OUTPUT_ASSERT("float", rect.GetWidth() == 5.f);
    };
    
    ssTEST("Return Object Test")
    {
        DummyClass assertObject(1, 2.0, "test");
        
        SO_OVERRIDE_RETURNS (OverrideObj, ReturnObjectFunc(int, double, std::string))
                            .Returns(assertObject);
        
        ssTEST_OUTPUT_ASSERT(ReturnObjectFunc(1, 3.0, "test 2") == assertObject);
    };
    
    ssTEST("Return String Test")
    {
        SO_OVERRIDE_RETURNS (OverrideObj, ReturnStringFunc(int))
                            .Returns(std::string("test"));
        
        ssTEST_OUTPUT_ASSERT(ReturnStringFunc(1) == "test");
    };
    
    ssTEST("Return Template Object Test")
    {
        DummyClass assertObject(1, 2.f, "test");
        DummyClass testObject(2, 3.f, "test 2");
        
        SO_OVERRIDE_RETURNS (OverrideObj, ReturnTemplateObjectFunc(T))
                            .Returns(assertObject);
        
        ssTEST_OUTPUT_ASSERT(ReturnTemplateObjectFunc<DummyClass>(testObject) == assertObject);
    };
    
    ssTEST("Return Nothing Test")
    {
        SO_OVERRIDE_RETURNS (OverrideObj, FuncWithoutArgs())
                            .Returns(SO_DONT_OVERRIDE_RETURN);

        ssTEST_OUTPUT_ASSERT(FuncWithoutArgs() == -1);
    };
    
    ssTEST_END();
    return 0;
}