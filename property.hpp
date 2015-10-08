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
            toFill = std::to_string(t);
        }
        
        void operator()(char & t) const
        {
            toFill = std::to_string(t);
        }
        
        void operator()(float & t) const
        {
            toFill = std::to_string(t);
        }
        
        void operator()(double & t) const
        {
            toFill = std::to_string(t);
        }
        
        void operator()(uint16_t & t) const
        {
            toFill = std::to_string(t);
        }
        
        void operator()(uint32_t & t) const
        {
            toFill = std::to_string(t);
        }
        
        void operator()(uint64_t & t) const
        {
            toFill = std::to_string(t);
        }
        
        void operator()(int32_t & t) const
        {
            toFill = std::to_string(t);
        }
        
        void operator()(int64_t & t) const
        {
            toFill = std::to_string(t);
        }
        
        void operator()(std::string & t) const
        {
            toFill = t;
        }
        
        void operator()(std::vector<float> & t) const
        {
            toFill = std::to_string(t[0]) + std::to_string(t[1]) + std::to_string(t[2]);
        }
};

//template <typename T>
//void propertyToString::operator()(const T& t) const {
//    toFill = std::to_string(t);
//}

std::string asString(value_type value);

void readProperties(dota::bitstream &stream, dt &serializer, Properties &props);

#endif /* _PROPERTY_HPP_ */