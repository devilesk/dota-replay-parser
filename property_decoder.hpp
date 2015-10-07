#ifndef _PROPERTY_DECODER_HPP_
#define _PROPERTY_DECODER_HPP_

#include "boost/variant.hpp"
#include "bitstream.hpp"
#include "property.hpp"
#include "quantizedfloat.hpp"
#include "parser_types.hpp"

extern std::unordered_map<dt_field*, QuantizedFloatDecoder> qmap;

value_type decodeLeUint64(dota::bitstream &stream, dt_field* f);
value_type decodeHandle(dota::bitstream &stream, dt_field* f);
value_type decodeByte(dota::bitstream &stream, dt_field* f);
value_type decodeShort(dota::bitstream &stream, dt_field* f);
value_type decodeUnsigned(dota::bitstream &stream, dt_field* f);
value_type decodeSigned(dota::bitstream &stream, dt_field* f);
value_type decodeSigned64(dota::bitstream &stream, dt_field* f);
value_type decodeFixed32(dota::bitstream &stream, dt_field* f);
value_type decodeFixed64(dota::bitstream &stream, dt_field* f);
value_type decodeBoolean(dota::bitstream &stream, dt_field* f);
value_type decodeFloat(dota::bitstream &stream, dt_field* f);
value_type decodeFloatNoscale(dota::bitstream &stream, dt_field* f);
value_type decodeQuantized(dota::bitstream &stream, dt_field* f);
value_type decodeSimTime(dota::bitstream &stream, dt_field* f);
value_type decodeString(dota::bitstream &stream, dt_field* f);
value_type decodeVector(dota::bitstream &stream, dt_field* f);
value_type decodeClass(dota::bitstream &stream, dt_field* f);
value_type decodeFVector(dota::bitstream &stream, dt_field* f);
value_type decodeNop(dota::bitstream &stream, dt_field* f);
value_type decodePointer(dota::bitstream &stream, dt_field* f);
value_type decodeQAngle(dota::bitstream &stream, dt_field* f);
value_type decodeComponent(dota::bitstream &stream, dt_field* f);
value_type decodeHSequence(dota::bitstream &stream, dt_field* f);

/*
uint32_t decodeLeUint64(dota::bitstream &stream, dt_field* f);
uint32_t decodeHandle(dota::bitstream &stream, dt_field* f);
char decodeByte(dota::bitstream &stream, dt_field* f);
uint16_t decodeShort(dota::bitstream &stream, dt_field* f);
uint64_t decodeUnsigned(dota::bitstream &stream, dt_field* f);
int32_t decodeSigned(dota::bitstream &stream, dt_field* f);
int64_t decodeSigned64(dota::bitstream &stream, dt_field* f);
uint32_t decodeFixed32(dota::bitstream &stream, dt_field* f);
uint64_t decodeFixed64(dota::bitstream &stream, dt_field* f);
bool decodeBoolean(dota::bitstream &stream, dt_field* f);
float decodeFloat(dota::bitstream &stream, dt_field* f);
float decodeFloatNoscale(dota::bitstream &stream, dt_field* f);


void decodeFloat(dota::bitstream &stream, dt_field* f);
void decodeSigned(dota::bitstream &stream, dt_field* f);
void decodeUnsigned(dota::bitstream &stream, dt_field* f);
void decodeString(dota::bitstream &stream, dt_field* f);
void decodeFVector(dota::bitstream &stream, dt_field* f);
void decodeBoolean(dota::bitstream &stream, dt_field* f);
void decodeQuantized(dota::bitstream &stream, dt_field* f);
void decodeComponent(dota::bitstream &stream, dt_field* f);
void decodeQAngle(dota::bitstream &stream, dt_field* f);
void decodeHandle(dota::bitstream &stream, dt_field* f);
void decodeVector(dota::bitstream &stream, dt_field* f);
void decodeSimTime(dota::bitstream &stream, dt_field* f);
*/
#endif /* _PROPERTY_DECODER_HPP_ */