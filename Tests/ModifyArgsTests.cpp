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
    
    ssTEST_SKIP("Modify Primitive types Test")
    {
    
    };
    
    ssTEST_SKIP("Modify Object Test")
    {
    
    };
    
    ssTEST_SKIP("Modify Template Object Test")
    {
    
    };
    
    ssTEST("Modify By Action Test")
    {
        SO_OVERRIDE_ARGS(OverrideObj, FuncWithArgsToSet(int, float*, std::string&))
                        .SetArgsByAction<int, float*, std::string&>
                        (
                            [](std::vector<void*>& args)
                            {
                                std::cout << "Called\n";
                                *((std::string*)args.at(2)) = "test";
                            }
                        );
        
        int testArg = 1;
        float testArg2 = 1.f;
        std::string testArg3 = "";
        
        FuncWithArgsToSet(testArg, &testArg2, testArg3);
        
        ssTEST_OUTPUT_ASSERT(testArg3 == "test");
    };
    
    ssTEST_SKIP("Modify Nothing Test")
    {
    
    };
    
    
    ssTEST_END();
    return 0;
}