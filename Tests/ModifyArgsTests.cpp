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
    
    ssTEST("Modify Primitive types Test")
    {
    
    };
    
    ssTEST("Modify Object Test")
    {
    
    };
    
    ssTEST("Modify Template Object Test")
    {
    
    };
    
    ssTEST("Modify Nothing Test")
    {
    
    };
    
    
    ssTEST_END();
    return 0;
}