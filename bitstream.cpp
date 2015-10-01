/**
 * @file bitstream.cpp
 * @author Robin Dietrich <me (at) invokr (dot) org>
 * @version 1.3
 *
 * @par License
 *    Alice Replay Parser
 *    Copyright 2014 Robin Dietrich
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "bitstream.hpp"

namespace dota {
    const uint64_t bitstream::masks[64] = {
        0x0,             0x1,              0x3,              0x7,
        0xf,             0x1f,             0x3f,             0x7f,
        0xff,            0x1ff,            0x3ff,            0x7ff,
        0xfff,           0x1fff,           0x3fff,           0x7fff,
        0xffff,          0x1ffff,          0x3ffff,          0x7ffff,
        0xfffff,         0x1fffff,         0x3fffff,         0x7fffff,
        0xffffff,        0x1ffffff,        0x3ffffff,        0x7ffffff,
        0xfffffff,       0x1fffffff,       0x3fffffff,       0x7fffffff,
        0xffffffff,      0x1ffffffff,      0x3ffffffff,      0x7ffffffff,
        0xfffffffff,     0x1fffffffff,     0x3fffffffff,     0x7fffffffff,
        0xffffffffff,    0x1ffffffffff,    0x3ffffffffff,    0x7ffffffffff,
        0xfffffffffff,   0x1fffffffffff,   0x3fffffffffff,   0x7fffffffffff,
        0xffffffffffff,  0x1ffffffffffff,  0x3ffffffffffff,  0x7ffffffffffff,
        0xfffffffffffff, 0x1fffffffffffff, 0x3fffffffffffff, 0x7fffffffffffff
    };

    const uint64_t bitstream::shift[64] = {
        0x0,  0x1,  0x2,  0x3,
        0x4,  0x5,  0x6,  0x7,
        0x8,  0x9,  0xa,  0xb,
        0xc,  0xd,  0xe,  0xf,
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b,
        0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23,
        0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b,
        0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33,
        0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b,
        0x3c, 0x3d, 0x3e, 0x3f
    };

    uint32_t bitstream::read(const bitstream::size_type n) {
        const uint32_t bitSize = sizeof(word_t)*8;        // size of chunk in bits
        const uint32_t start   = pos / bitSize;           // current active chunk
        const uint32_t end     = (pos + n - 1) / bitSize; // next chunk if data needs to be wrapped
        const uint32_t s       = (pos % bitSize);         // shift amount
        uint32_t ret;                                     // return value

        if (start == end) {
            ret = (data[start] >> shift[s]) & masks[n];
        } else { // wrap around
            ret = ((data[start] >> shift[s]) | (data[end] << (bitSize - shift[s]))) & masks[n];
        }

        pos += n;
        return ret;
    }
    
    float bitstream::nReadAngle(uint32_t n) {
      return (float)(read((int)n)) * 360.0 / (float)((int)(1 << n));
    }
    
    std::vector<float> bitstream::nRead3BitNormal() {
      std::vector<float> ret { 0.0, 0.0, 0.0 };
      bool hasX = read(1) != 0;
      bool hasY = read(1) != 0;
      
      if (hasX) {
        ret[0] = nReadNormal();
      }
      
      if (hasY) {
        ret[1] = nReadNormal();
      }
      
      bool negZ = read(1) != 0;
      float prodsum = ret[0] * ret[0] + ret[1] * ret[1];
      
      if (prodsum < 1.0) {
        ret[2] = (float)sqrt((double)(1.0 - prodsum));
      }
      else {
        ret[2] = 0.0;
      }
      
      if (negZ) {
        ret[2] = -ret[2];
      }
      
      return ret;
    }
    
    /*float nReadNormal() {
      bool isNeg = read(1) != 0;
      uint32_t len = read(11);
      float ret = (float)len * (float)(1.0 / ((float)(1 << 11) - 1.0));
      
      if (isNeg) {
        return -ret;
      }
      else {
        return ret;
      }
    }*/

    uint16_t bitstream::nReadLeUInt16() {
      return boost::endian::native_to_little<uint16_t>(static_cast<uint16_t>(read(16)));
    }
    
    uint32_t bitstream::nReadLeUInt32() {
      return boost::endian::native_to_little<uint32_t>(read(32));
    }
    
    // TODO: check endian order
    uint64_t bitstream::nReadLeUInt64() {
      char buf[64];
      readBits(buf, 64);
      uint64_t value = 
        static_cast<uint64_t>(buf[0]) |
        static_cast<uint64_t>(buf[1]) << 8 |
        static_cast<uint64_t>(buf[2]) << 16 |
        static_cast<uint64_t>(buf[3]) << 24 |
        static_cast<uint64_t>(buf[4]) << 32 |
        static_cast<uint64_t>(buf[5]) << 40 |
        static_cast<uint64_t>(buf[6]) << 48 |
        static_cast<uint64_t>(buf[7]) << 56;
      return value;
    }
    
    uint16_t bitstream::nReadBeUInt16() {
      return boost::endian::native_to_big<uint16_t>(static_cast<uint16_t>(read(16)));
    }
    
    uint32_t bitstream::nReadBeUInt32() {
      return boost::endian::native_to_big<uint32_t>(read(32));
    }
    
    // TODO: check endian order
    uint64_t bitstream::nReadBeUInt64() {
      char buf[64];
      readBits(buf, 64);
      uint64_t value = 
        static_cast<uint64_t>(buf[7]) |
        static_cast<uint64_t>(buf[6]) << 8 |
        static_cast<uint64_t>(buf[5]) << 16 |
        static_cast<uint64_t>(buf[4]) << 24 |
        static_cast<uint64_t>(buf[3]) << 32 |
        static_cast<uint64_t>(buf[2]) << 40 |
        static_cast<uint64_t>(buf[1]) << 48 |
        static_cast<uint64_t>(buf[0]) << 56;
      return value;
    }
    
    uint32_t bitstream::nReadUBitVar() {
      uint32_t value;
      value = read(6);
      switch (value & 0x30) {
      case 16:
        value = (value & 15) | (read(4) << 4);
        break;
      case 32:
        value = (value & 15) | (read(8) << 4);
        break;
      case 48:
        value = (value & 15) | (read(28) << 4);
        break;
      }
      return value;
    }
    
    uint32_t bitstream::nReadUBitVarFP() {
      if (read(1) != 0) {
        return read(2);
      }
      if (read(1) != 0) {
        return read(4);
      }
      if (read(1) != 0) {
        return read(10);
      }
      if (read(1) != 0) {
        return read(17);
      }
      return read(31);
    }
    
    uint32_t bitstream::nReadVarUInt32() {
        uint32_t readCount = 0;
        uint32_t value = 0;

        uint32_t tmpBuf;
        do {
            if (readCount == VARINT32_MAX) // return when maximum number of iterations is reached
                return value;

            tmpBuf = read(8);
            value |= (tmpBuf & 0x7F) << (7*readCount);
            ++readCount;
        } while (tmpBuf & 0x80);

        return value;
    }
    
    uint64_t bitstream::nReadVarUInt64() {
        uint32_t readCount = 0;
        uint64_t value = 0;

        uint32_t tmpBuf;
        do {
            if (readCount == VARINT64_MAX)
                return value;

            tmpBuf = read(8);
            value |= static_cast<uint64_t>(tmpBuf & 0x7F) << (7*readCount);
            ++readCount;
        } while (tmpBuf & 0x80);

        return value;
    }

    float bitstream::nReadCoord() {
        uint32_t intval   = read(1); // integer part
        uint32_t fractval = read(1); // fraction part

        if ( intval || fractval ) {
            float ret  = 0.0f; // return value
            const bool sign = read(1); // signed / unsigned flag

            if (intval)
                intval = read( COORD_INTEGER_BITS ) + 1;

            if (fractval)
                // shift from [0..MAX-1] to (1..MAX)
                fractval = read( COORD_FRACTION_BITS );

            ret = intval + (static_cast<float>(fractval) * COORD_RESOLUTION);

            if (sign)
                return -ret;
            else
                return ret;
        }

        return 0.0f;
    }
    float bitstream::nReadCoordMp(const bool integral, const bool lowPrecision) {
        // Read flags depending on type
        const uint32_t flags = integral ? read(3) : read(2);
        const uint32_t flag_inbound = 1;
        const uint32_t flag_intval  = 2;
        const uint32_t flag_sign    = 4;

        if (integral) {
            if (flags & flag_intval) {
                const uint32_t toRead = (flags & flag_inbound) ? COORD_INTEGER_BITS_MP+1 : COORD_INTEGER_BITS+1;
                const uint32_t bits = read(toRead);

                // shift from [0..MAX-1] to [1..MAX], fist part of bits is the sign type
                const int intval = (bits >> 1) + 1;
                return (bits & 1) ? -intval : intval;
            }

            return 0.0f;
        }

        // multiplication table
        static const float mult[4] = {
             1.f / (1 << COORD_FRACTION_BITS),
            -1.f / (1 << COORD_FRACTION_BITS),
             1.f / (1 << COORD_FRACTION_BITS_MP_LOWPRECISION),
            -1.f / (1 << COORD_FRACTION_BITS_MP_LOWPRECISION)
        };

        // multiplication val
        const float multiply = mult[ (flags & flag_sign ? 1 : 0) + lowPrecision*2];

        // bits to read depending on type
        static const unsigned char bits[8] = {
            COORD_FRACTION_BITS,
            COORD_FRACTION_BITS,
            COORD_FRACTION_BITS + COORD_INTEGER_BITS,
            COORD_FRACTION_BITS + COORD_INTEGER_BITS_MP,
            COORD_FRACTION_BITS_MP_LOWPRECISION,
            COORD_FRACTION_BITS_MP_LOWPRECISION,
            COORD_FRACTION_BITS_MP_LOWPRECISION + COORD_INTEGER_BITS,
            COORD_FRACTION_BITS_MP_LOWPRECISION + COORD_INTEGER_BITS_MP
        };

        uint32_t val = read( bits[ (flags & (flag_inbound|flag_intval)) + lowPrecision*4] );

        if (flags & flag_intval) {
            // Remap the integer part from [0,N] to [1,N+1] and paste
            //  it in front of the fractional parts.

            const uint32_t fracMp  = val >> COORD_INTEGER_BITS_MP;
            uint32_t frac          = val >> COORD_INTEGER_BITS;

            const uint32_t maskMp  = ((1<<COORD_INTEGER_BITS_MP)-1);
            uint32_t mask          = ((1<<COORD_INTEGER_BITS)-1);

            uint32_t selectNotMp = (flags & flag_inbound) - 1;

            frac -= fracMp;
            frac &= selectNotMp;
            frac += fracMp;

            mask -= maskMp;
            mask &= selectNotMp;
            mask += maskMp;

            const uint32_t intpart      = (val & mask) + 1;
            const uint32_t intbitsLow   = intpart << COORD_FRACTION_BITS_MP_LOWPRECISION;
            uint32_t intbits            = intpart << COORD_FRACTION_BITS;
            const uint32_t selectNotLow = static_cast<uint32_t>(lowPrecision) - 1;

            intbits -= intbitsLow;
            intbits &= selectNotLow;
            intbits += intbitsLow;

            val = frac | intbits;
        }

        return static_cast<int32_t>(val) * multiply;
    }

    void bitstream::nSkipCoordMp(const bool integral, const bool lowPrecision) {
        // Read flags depending on type
        const uint32_t flags = integral ? read(3) : read(2);
        const uint32_t flag_inbound = 1;
        const uint32_t flag_intval  = 2;

        if (integral && (flags & flag_intval)) {
            seekForward( (flags & flag_inbound) ? COORD_INTEGER_BITS_MP+1 : COORD_INTEGER_BITS+1 );
            return;
        }

        // bits to skip depending on type
        static const unsigned char bits[8] = {
            COORD_FRACTION_BITS,
            COORD_FRACTION_BITS,
            COORD_FRACTION_BITS + COORD_INTEGER_BITS,
            COORD_FRACTION_BITS + COORD_INTEGER_BITS_MP,
            COORD_FRACTION_BITS_MP_LOWPRECISION,
            COORD_FRACTION_BITS_MP_LOWPRECISION,
            COORD_FRACTION_BITS_MP_LOWPRECISION + COORD_INTEGER_BITS,
            COORD_FRACTION_BITS_MP_LOWPRECISION + COORD_INTEGER_BITS_MP
        };

        seekForward( bits[ (flags & (flag_inbound|flag_intval)) + lowPrecision*4] );
    }

    float bitstream::nReadCellCoord(size_type n, const bool integral, const bool lowPrecision) {
        uint32_t val = read(n);

        if (integral) {
            if (val & 0x80)
                val += 4.2949673e9;

            return (float)val;
        } else {
            uint32_t fraction = lowPrecision ? read(CELL_COORD_FRACTION_BITS_LOWPRECISION) : read(CELL_COORD_FRACTION_BITS);
            float f = val + (lowPrecision ? 0.125 : 0.03125) * fraction;

            return f;
        }
    }

    void bitstream::nReadString(char *buffer, const bitstream::size_type n) {
        for (std::size_t i = 0; i < n; ++i) {
            buffer[i] = static_cast<char>(read(8));

            if (buffer[i] == '\0')
                return;
        }

        buffer[n - 1] = '\0';
    }

    void bitstream::readBits(char *buffer, const bitstream::size_type n) {
        size_type remaining = n;
        size_type i = 0;

        while (remaining >= 8) {
            buffer[i++] = read(8);
            remaining -= 8;
        }

        if (remaining > 0) {
            buffer[i++] = read(remaining);
        }
    }
}

uint32_t readVarUInt32(std::ifstream &stream) {
  uint32_t i = 0;
  uint32_t value = 0;
  unsigned char b;
  do {
    stream.read((char*)&b, 1);
    value |= ((int)b & 0x7F) << (7 * i);
    ++i;
  } while (b & 0x80 && i < 35);
  return value;
}

uint32_t readVarUInt32(const char* buffer, int* pos) {
  uint32_t i = 0;
  uint32_t value = 0;
  unsigned char b;
  do {
    b = buffer[i];
    value |= ((int)b & 0x7F) << (7 * i);
    ++i;
  } while (b & 0x80 && i < 5);
  (*pos) += i;
  return value;
}