#ifndef _PROPERTY_HPP_
#define _PROPERTY_HPP_

#include <iostream>

#include "boost/variant.hpp"
#include "bitstream.hpp"
#include "parser_types.hpp"
#include "fieldpath.hpp"

class propertyToString : public boost::static_visitor<> {
    private:
        /** String to fill */
        std::string &toFill;
    public:
        /** Sets internal string as a reference to r */
        propertyToString(std::string &r) : toFill(r) {}

        /** Invoking this does the actual string conversion */
        //template <typename T>
        //void operator()(const T& t) const;
        
        void operator()(bool & t) const
        {
            std::cout << "bool\n";
            toFill = std::to_string(t);
        }
        
        void operator()(char & t) const
        {
            std::cout << "char\n";
            toFill = std::to_string(t);
        }
        
        void operator()(float & t) const
        {
            std::cout << "float\n";
            toFill = std::to_string(t);
        }
        
        void operator()(double & t) const
        {
            std::cout << "double\n";
            toFill = std::to_string(t);
        }
        
        void operator()(uint16_t & t) const
        {
            std::cout << "uint16_t\n";
            toFill = std::to_string(t);
        }
        
        void operator()(uint32_t & t) const
        {
            std::cout << "uint32_t\n";
            toFill = std::to_string(t);
        }
        
        void operator()(uint64_t & t) const
        {
            std::cout << "uint64_t\n";
            toFill = std::to_string(t);
        }
        
        void operator()(int32_t & t) const
        {
            std::cout << "int32_t\n";
            toFill = std::to_string(t);
        }
        
        void operator()(int64_t & t) const
        {
            std::cout << "int64_t\n";
            toFill = std::to_string(t);
        }
        
        void operator()(std::string & t) const
        {
            std::cout << "std::string\n";
            toFill = t;
        }
        
        void operator()(std::vector<float> & t) const
        {
            std::cout << "std::vector<float>\n";
            toFill = std::to_string(t[0]) + std::to_string(t[1]) + std::to_string(t[2]);
        }
};

//template <typename T>
//void propertyToString::operator()(const T& t) const {
//    toFill = std::to_string(t);
//}

std::string asString(value_type value);

std::shared_ptr<Properties> readProperties(dota::bitstream &stream, dt* serializer);

#endif /* _PROPERTY_HPP_ */