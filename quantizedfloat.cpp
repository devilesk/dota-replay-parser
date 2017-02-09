#include "quantizedfloat.hpp"

const uint32_t qff_rounddown = (1 << 0);
const uint32_t qff_roundup = (1 << 1);
const uint32_t qff_encode_zero = (1 << 2);
const uint32_t qff_encode_integers = (1 << 3);

// Validates / recomputes decoder flags
void validateFlags(QuantizedFloatDecoder* qfd) {
    if (qfd->field->has_flags) {
        qfd->flags = (uint32_t)(qfd->field->flags);
    }
    else {
        qfd->flags = 0;
        return;
    }
    
    // Discard zero flag when encoding min / max set to 0
    if ((qfd->low == 0.0 && (qfd->flags & qff_rounddown) != 0) || (qfd->high == 0.0 && (qfd->flags & qff_roundup) != 0)) {
        qfd->flags &=  qfd->flags ^ qff_encode_zero;
    }
    
    // If min / max is zero when encoding zero, switch to round up / round down instead
    if (qfd->low == 0.0 && (qfd->flags & qff_encode_zero) != 0) {
        qfd->flags |= qff_rounddown;
        qfd->flags &= qfd->flags ^ qff_encode_zero;
    }
    
    if (qfd->high == 0.0 && (qfd->flags & qff_encode_zero) != 0) {
        qfd->flags |= qff_roundup;
        qfd->flags &= qfd->flags ^ qff_encode_zero;
    }
    
    // Check if the range spans zero
    if (qfd->low > 0.0 || qfd->high < 0.0) {
        qfd->flags &= qfd->flags ^ qff_encode_zero;
    }
    
    // If we are left with encode zero, only leave integer flag
    if ((qfd->flags & qff_encode_integers) != 0) {
        qfd->flags &= qfd->flags ^ (qff_roundup | qff_rounddown | qff_encode_zero);
    }
    
    // Verify that we don;t have roundup / rounddown set
    if ((qfd->flags & (qff_rounddown | qff_roundup)) == (qff_rounddown | qff_roundup)) {
        // throw exception
    }
}

// Assign multipliers
void assignMultipliers(QuantizedFloatDecoder* qfd, uint32_t steps) {
    qfd->highLowMul = 0.0;
    float range = qfd->high - qfd->low;
    uint32_t high = 0;
    if (qfd->bitcount == 32) {
        high = 0xFFFFFFFE;
    }
    else {
        high = (1 << qfd->bitcount) - 1;
    }
    
    float highMul = (float)0.0;
    if (std::abs((double)range) <= 0.0) {
        highMul = (float)high;
    }
    else {
        highMul = (float)high / range;
    }
    
    // Adjust precision
    if ((highMul * range > (float)high) || ((double)(highMul * range) > (double)high)) {
        float multipliers[5] = { 0.9999, 0.99, 0.9, 0.8, 0.7 };
        
        for (int i = 0; i < 5; ++i) {
            highMul = (float)high / range * multipliers[i];
            
            if ((highMul * range > (float)high) || ((double)(highMul * range) > (double)high)) {
                continue;
            }
            
            break;
        }
    }
    
    qfd->highLowMul = highMul;
    qfd->decMul = 1.0 / (float)(steps - 1);
    
    if (qfd->highLowMul == 0.0) {
        // throw exception;
        //std::cout << "Error computing high / low multiplier\n";
    }
}

// Quantize a float
float quantize(QuantizedFloatDecoder* qfd, float val) {
    if (val < qfd->low) {
        if (((uint32_t)(qfd->field->flags) & qff_roundup) == 0) {
            //std::cout << "Field tried to quantize an out of range value\n";
        }
        //std::cout << "quantize returning low " << std::to_string(qfd->low) << "\n";
        return qfd->low;
    }
    else if (val > qfd->high) {
        if (((uint32_t)(qfd->field->flags) & qff_rounddown) == 0) {
            //std::cout << "Field tried to quantize an out of range value\n";
        }
        //std::cout << "quantize returning high " << std::to_string(val) << "\n";
        //std::cout << "quantize returning high " << std::to_string(qfd->low) << "\n";
        //std::cout << "quantize returning high " << std::to_string(qfd->high) << "\n";
        return qfd->high;
    }
    
    uint32_t i = (uint32_t)((val - qfd->low) * qfd->highLowMul);
    //std::cout << "quantize HighLowMul " << std::to_string(qfd->highLowMul) << "\n";
    //std::cout << "quantize val - qfd.Low " << std::to_string(val - qfd->low) << "\n";
    //std::cout << "quantize i " << std::to_string(i) << "\n";
    //std::cout << "quantize " << std::to_string(qfd->low + (qfd->high - qfd->low) * ((float)(i) * qfd->decMul)) << "\n";
    return qfd->low + (qfd->high - qfd->low) * ((float)(i) * qfd->decMul);
}

// Actual float decoding
float decode(QuantizedFloatDecoder* qfd, dota::bitstream &stream) {
    if ((qfd->flags & qff_rounddown) != 0 && stream.read(1) != 0) {
        return qfd->low;
    }
    
    if ((qfd->flags & qff_roundup) != 0 && stream.read(1) != 0) {
        return qfd->high;
    }
    
    if ((qfd->flags & qff_encode_zero) != 0 && stream.read(1) != 0) {
        return 0.0;
    }
    
    return qfd->low + (qfd->high - qfd->low) * (float)(stream.read((int)(qfd->bitcount))) * qfd->decMul;
}

// Creates a quantized float decoder based on given field
QuantizedFloatDecoder initQFD(dt_field* f) {
    QuantizedFloatDecoder qfd;
    qfd.field = f;
    
    // Set common properties
    if (f->bit_count == 0 || f->bit_count >= 32) {
        qfd.noScale = true;
        qfd.bitcount = 32;
        return qfd;
    }
    else {
        qfd.noScale = false;
        qfd.bitcount = (uint32_t)(f->bit_count);
        qfd.offset = 0.0;
        
        if (f->has_low_value) {
            qfd.low = f->low_value;
        }
        else {
            qfd.low = 0.0;
        }
        
        if (f->has_high_value) {
            qfd.high = f->high_value;
        }
        else {
            qfd.high = 1.0;
        }
    }
    //std::cout << "initQFD high value %s " << std::to_string(qfd.high) << "\n";
    
    // Validate flags
    validateFlags(&qfd);
    
    // Handle Round Up, Round Down
    uint32_t steps = (1 << (uint32_t)qfd.bitcount);
    
    //std::cout << "initQFD bitcount %s " << std::to_string(qfd.bitcount) << "\n";
    //std::cout << "initQFD steps %s " << std::to_string(steps) << "\n";
    
    float range = 0;
    
    if ((qfd.flags & qff_rounddown) != 0) {
        range = qfd.high - qfd.low;
        qfd.offset = (range / (float)steps);
        qfd.high -= qfd.offset;
        //std::cout << "initQFD add offset to high %s " << std::to_string(qfd.offset) << "\n";
    }
    else if ((qfd.flags & qff_roundup) != 0) {
        range = qfd.high - qfd.low;
        qfd.offset = (range / (float)steps);
        qfd.low += qfd.offset;
    }
    
    // Handle integer encoding flag
    if ((qfd.flags & qff_encode_integers) != 0) {
        float delta = qfd.high - qfd.low;
        
        if (delta < 1) {
            delta = 1;
        }
        
        double deltaLog2 = ceil(log2((double)delta));
        uint32_t range2 = (1 << (uint32_t)deltaLog2);
        uint32_t bc = qfd.bitcount;
        
        //for 1 == 1 {
            if ((uint32_t)(1 << (uint32_t)bc) > range2) {
                //break;
            }
            else {
                ++bc;
            }
        //}
        
        if (bc > qfd.bitcount) {
            //std::cout << "Upping bitcount for qf_encode_integers field " << std::to_string(qfd.bitcount) << " -> " << bc << "\n";
            qfd.bitcount = bc;
            steps = (1 << (uint32_t)qfd.bitcount);
        }
        
        qfd.offset = ((float)range2) / ((float)steps);
        qfd.high = qfd.low + (float)range2 - qfd.offset;
    }
    
    // Assign multipliers
    assignMultipliers(&qfd, (uint32_t)steps);
    
    // Remove unnecessary flags
    if ((qfd.flags & qff_rounddown) != 0) {
        if (quantize(&qfd, qfd.low) == qfd.low) {
            qfd.flags &= qfd.flags ^ qff_rounddown;
            //std::cout << "initQFD remove rounddown\n";
        }
    }
    
    if ((qfd.flags & qff_roundup) != 0) {
        if (quantize(&qfd, qfd.high) == qfd.high) {
            qfd.flags &= qfd.flags ^ qff_roundup;
            //std::cout << "initQFD remove roundup\n";
        }
    }
    
    if ((qfd.flags & qff_encode_zero) != 0) {
        if (quantize(&qfd, 0.0) == 0.0) {
            qfd.flags &= qfd.flags ^ qff_encode_zero;
            //std::cout << "initQFD remove qff_encode_zero\n";
        }
    }
    
    return qfd;
}