#include "property_decoder.hpp"

value_type decodeLeUint64(dota::bitstream &stream, dt_field* f) {
  return stream.nReadBeUInt64();
}
value_type decodeHandle(dota::bitstream &stream, dt_field* f) {
  return stream.nReadVarUInt32();
}
value_type decodeByte(dota::bitstream &stream, dt_field* f) {
  return stream.read(8);
}
value_type decodeShort(dota::bitstream &stream, dt_field* f) {
  return stream.read(16);
}
value_type decodeUnsigned(dota::bitstream &stream, dt_field* f) {
  if (f->encoder.compare("fixed64") == 0) {
    return decodeLeUint64(stream, f);
  }
  return stream.nReadVarUInt64();
}
value_type decodeSigned(dota::bitstream &stream, dt_field* f) {
  return stream.nReadVarSInt32();
}
value_type decodeSigned64(dota::bitstream &stream, dt_field* f) {
  return stream.nReadVarSInt64();
}
value_type decodeFixed32(dota::bitstream &stream, dt_field* f) {
  return stream.read(32);
}
value_type decodeFixed64(dota::bitstream &stream, dt_field* f) {
  uint64_t ret = (uint64_t)stream.read(32);
  ret = (ret << 32) | (uint64_t)stream.read(32);
  return ret;
}
value_type decodeBoolean(dota::bitstream &stream, dt_field* f) {
  return stream.read(1) != 0;
}
value_type decodeFloat(dota::bitstream &stream, dt_field* f) {
  if (f->encoder.compare("coord") == 0) {
    return stream.nReadCoord();
  }
  
  if (f->has_bit_count == 0 || (f->bit_count <= 0 || f->bit_count >= 32)) {
    return decodeFloatNoscale(stream, f);
  }
  
  return decodeQuantized(stream, f);
}
value_type decodeFloatNoscale(dota::bitstream &stream, dt_field* f) {
  static_assert(sizeof(float) == sizeof(uint32_t), "size of float not equal to size of uint32_t");
  uint32_t value = stream.read(32);
  float* pFloat = reinterpret_cast<float*>(&value);
  float ret = *pFloat;
  return ret;
}

std::map<dt_field*, QuantizedFloatDecoder> qmap;

value_type decodeQuantized(dota::bitstream &stream, dt_field* f) {
  QuantizedFloatDecoder q;
  
  // Get the correct decoder
  if (qmap.find(f) == qmap.end()) {
    qmap[f] = initQFD(f);
  }
  q = qmap[f];
  
  // Decode value
  std::cout << "Bitcount: " << std::to_string(q.bitcount) << ", Low: " << std::to_string(q.low) << ", High: " << std::to_string(q.high) << ", Flags: " << std::to_string(q.flags) << "\n";
  
  return decode(&q, stream);
}
value_type decodeSimTime(dota::bitstream &stream, dt_field* f) {
  return (float)(stream.nReadVarUInt32()) * (1.0 / 30);
}
value_type decodeString(dota::bitstream &stream, dt_field* f) {
  char buf[1024];
  std::string ret = std::string(buf);
  stream.nReadString(buf, 1024);
  return ret;
}
value_type decodeVector(dota::bitstream &stream, dt_field* f) {
  return stream.nReadVarUInt32();
}
value_type decodeClass(dota::bitstream &stream, dt_field* f) {
  return stream.nReadVarUInt32();
}
value_type decodeFVector(dota::bitstream &stream, dt_field* f) {
  // Parse specific encoders
  if (f->encoder.compare("normal") == 0) {
    return stream.nRead3BitNormal();
  }
  
  std::vector<float> ret { (float)boost::get<float>(decodeFloat(stream, f)), (float)boost::get<float>(decodeFloat(stream, f)), (float)boost::get<float>(decodeFloat(stream, f)) };
  return ret;
}
value_type decodeNop(dota::bitstream &stream, dt_field* f) {
  return 0;
}
value_type decodePointer(dota::bitstream &stream, dt_field* f) {
	// Seems to be encoded as a single bit, not sure what to make of it
	if (stream.read(1) == 0) {
		std::cout << "Figure out how this works\n";
	}

	return 0;
}
value_type decodeQAngle(dota::bitstream &stream, dt_field* f) {
  std::vector<float> ret { 0.0, 0.0, 0.0 };
  
  // Parse specific encoders
  if (f->encoder.compare("qangle_pitch_yaw") == 0) {
    if (f->has_bit_count && f->has_flags && (f->flags & 0x20 != 0)) {
      std::cout << "Special Case: Unknown for now\n";
    }
    
    ret[0] = stream.nReadAngle((uint32_t)f->bit_count);
    ret[1] = stream.nReadAngle((uint32_t)f->bit_count);
    return ret;
  }
  
  // Parse a standard angle
  if (f->has_bit_count && f->bit_count == 32) {
    std::cout << "Special Case: Unknown for now\n";
  }
  else if (f->has_bit_count && f->bit_count != 0) {
    ret[0] = stream.nReadAngle((uint32_t)f->bit_count);
    ret[1] = stream.nReadAngle((uint32_t)f->bit_count);
    ret[2] = stream.nReadAngle((uint32_t)f->bit_count);
    
    return ret;
  }
  else {
    bool rX = stream.read(1) != 0;
    bool rY = stream.read(1) != 0;
    bool rZ = stream.read(1) != 0;
    
    if (rX) {
      ret[0] = stream.nReadCoord();
    }
    
    if (rY) {
      ret[1] = stream.nReadCoord();
    }
    
    if (rZ) {
      ret[2] = stream.nReadCoord();
    }
    
    return ret;
  }
  
  std::cout << "No valid encoding determined\n";
  return ret;
}
value_type decodeComponent(dota::bitstream &stream, dt_field* f) {
	std::cout << "Bitcount: " << std::to_string(f->bit_count) << ", Low: " << std::to_string(f->low_value) << ", High: " << std::to_string(f->high_value) << ", Flags: " << std::to_string(f->flags) << "\n";

	return stream.read(1);
}
value_type decodeHSequence(dota::bitstream &stream, dt_field* f) {
	// wrong, just testing
	return stream.read(1);
}
/*
uint32_t decodeHandle(dota::bitstream &r, dt_field* f) {
  return r.nReadVarUInt32();
}

char decodeByte(dota::bitstream &r, dt_field* f) {
  return (char)r.read(8);
}

uint16_t decodeShort(dota::bitstream &r, dt_field* f) {
  return (int16_t)r.read(16);
}

int32_t decodeSigned(dota::bitstream &r, dt_field* f) {
  return r.nReadVarSInt32();
}

int64_t decodeSigned64(dota::bitstream &r, dt_field* f) {
  return r.nReadVarSInt64();
}

uint32_t decodeFixed32(dota::bitstream &r, dt_field* f) {
  return r.read(32);
}

uint64_t decodeFixed64(dota::bitstream &r, dt_field* f) {
  uint64_t ret = (uint64_t)r.read(32);
  ret = (ret << 32) | (uint64_t)(r.read(32));
  return ret;
}

bool decodeBoolean(dota::bitstream &r, dt_field* f) {
  return r.read(1) != 0;
}


void decodeFloat(dota::bitstream &stream, dt_field* f) {}
void decodeSigned(dota::bitstream &stream, dt_field* f) {}
void decodeUnsigned(dota::bitstream &stream, dt_field* f) {}
void decodeString(dota::bitstream &stream, dt_field* f) {}
void decodeFVector(dota::bitstream &stream, dt_field* f) {}
void decodeBoolean(dota::bitstream &stream, dt_field* f) {}
void decodeQuantized(dota::bitstream &stream, dt_field* f) {}
void decodeComponent(dota::bitstream &stream, dt_field* f) {}
void decodeQAngle(dota::bitstream &stream, dt_field* f) {}
void decodeHandle(dota::bitstream &stream, dt_field* f) {}
void decodeVector(dota::bitstream &stream, dt_field* f) {}
void decodeSimTime(dota::bitstream &stream, dt_field* f) {}
*/