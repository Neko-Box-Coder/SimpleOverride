#ifndef SO_DUMMY_CLASS_HPP
#define SO_DUMMY_CLASS_HPP

#include <iostream>

class DummyClass {
    private:
        int data;
        double value;
        std::string name;

    public:
        // Constructor
        DummyClass() : data(0), value(0.0), name("") { }
        DummyClass(int d, double v, std::string n) : data(d), value(v), name(n) {}

        // Copy constructor
        DummyClass(const DummyClass& other) :   data(other.data), 
                                                value(other.value), 
                                                name(other.name)
        {}

        // Comparison operator
        inline bool operator==(const DummyClass& other) const 
        {
            return (data == other.data) && (value == other.value) && (name == other.name);
        }
        
        inline bool operator!=(const DummyClass& other) const
        {
            return DummyClass::operator==(other);
        }

        inline void SetData(int d)
        {
            data = d;
        }
        
        inline int GetData() const 
        {
            return data;
        }
        
        inline void SetValue(double v) 
        {
            value = v;
        }

        inline double GetValue() const 
        {
            return value;
        }
        
        inline void SetName(const std::string& n)
        {
            name = n;
        }

        inline std::string GetName() const 
        {
            return name;
        }
};

template <typename T>
class TemplateDummy : public DummyClass
{
    private:
        T GenericData;
    
    public:
        inline TemplateDummy(T genericData, int d, double v, std::string n) :   
            DummyClass(d, v, n),
            GenericData(genericData)
        {}
        
        inline TemplateDummy(T genericData) :   DummyClass(),
                                                GenericData(genericData)
        {}

        inline bool operator ==(const TemplateDummy<T>& other) const
        {
            return  DummyClass::operator==(other) && 
                    GenericData == other.GenericData;
        }
        
        inline void SetGenericData(T genericData)
        {
            GenericData = genericData;
        }
        
        inline T GetGenericData() const
        {
            return GenericData;
        }
};


#endif