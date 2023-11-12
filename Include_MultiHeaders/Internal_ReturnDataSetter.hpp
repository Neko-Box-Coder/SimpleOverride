#ifndef SO_INTERNAL_RETURN_DATA_SETTER_HPP
#define SO_INTERNAL_RETURN_DATA_SETTER_HPP

#include "./ProxiesDeclarations.hpp"
#include "./Internal_ReturnData.hpp"
#include "./Internal_OverrideReturnDataInfo.hpp"
#include "./ProxiesDeclarations.hpp"

#include <unordered_map>

namespace SimpleOverride
{
    class Internal_ReturnDataSetter
    {
        friend class ReturnProxy;
    
        public:
            using ReturnInfosType = std::unordered_map<std::string, Internal_OverrideReturnDataInfo>;
        
        protected:
            ReturnInfosType& OverrideReturnInfos;
            
            //------------------------------------------------------------------------------
            //Methods for setting return data
            //------------------------------------------------------------------------------
            template<typename T>
            inline ReturnProxy& ReturnsByAction(ReturnProxy& proxy, 
                                                                std::function<void(std::vector<void*>& args, void* out)> returnAction)
            {
                Internal_ReturnData& lastData = OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back();

                lastData.ReturnDataInfo.DataAction = returnAction;
                lastData.ReturnDataInfo.DataActionSet = true;
                lastData.ReturnDataInfo.DataType = typeid(T).hash_code();
                return proxy;
            }
        
            template<typename T>
            inline ReturnProxy& Returns(ReturnProxy& proxy, T returnData)
            {
                Internal_ReturnData& lastData = OverrideReturnInfos[proxy.FunctionSignatureName].ReturnDatas.back();
                lastData.ReturnDataInfo.Data = new T(returnData);
                lastData.ReturnDataInfo.CopyConstructor = [](void* data) { return new T(*static_cast<T*>(data)); };
                lastData.ReturnDataInfo.Destructor = [](void* data) { delete static_cast<T*>(data); }; 
                lastData.ReturnDataInfo.DataSet = true;
                lastData.ReturnDataInfo.DataType = typeid(T).hash_code();
                return proxy;
            }
        
        public:
            inline Internal_ReturnDataSetter(ReturnInfosType& overrideReturnInfos) : 
                OverrideReturnInfos(overrideReturnInfos)
            {}
    };
}


#endif