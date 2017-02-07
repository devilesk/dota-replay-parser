#ifndef _QUANTIZEDFLOAT_HPP_
#define _QUANTIZEDFLOAT_HPP_

#include <iostream>
#include <math.h>

#include "bitstream.hpp"
#include "parser_types.hpp"

extern const uint32_t qff_rounddown;
extern const uint32_t qff_roundup;
extern const uint32_t qff_encode_zero;
extern const uint32_t qff_encode_integers;

struct QuantizedFloatDecoder {
    dt_field* field; // points to datatable field
    float low;  // Gets recomputed for round up / down
    float high;
    float highLowMul;
    float decMul;
    float offset;
    uint32_t bitcount;
    uint32_t flags;
    bool noScale;
};

void validateFlags(QuantizedFloatDecoder* qfd);
void assignMultipliers(QuantizedFloatDecoder* qfd, uint32_t steps);
float quantize(QuantizedFloatDecoder* qfd, float val);
float decode(QuantizedFloatDecoder* qfd, dota::bitstream &stream);
QuantizedFloatDecoder initQFD(dt_field* f);

#endif /* _QUANTIZEDFLOAT_HPP_ */