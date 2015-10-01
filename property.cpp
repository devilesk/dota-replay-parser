#include "property.hpp"

std::string asString(value_type value) {
    std::string ret;
    propertyToString v(ret);
    boost::apply_visitor( v, value );

    return ret;
}

Properties readProperties(dota::bitstream &stream, dt &serializer) {
  // return type
  Properties result;
  
  // Create fieldpath
  fieldpath fp = newFieldpath(&serializer, &huf);
  
  // Get a list of the included fields
  walk(stream, &fp);
  
  // iterate all the fields and set their corresponding values
  for (int i = 0; i < fp.fields.size(); ++i) {
    std::cout << "Decoding field: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << fp.fields[i].field->encoder << "\n";
    if (fp.fields[i].field->serializer.decodeContainer != nullptr) {
      std::cout << "Decoding container: " << fp.fields[i].field->name << "\n";
      result.KV[fp.fields[i].field->name] = fp.fields[i].field->serializer.decodeContainer(stream, fp.fields[i].field);
    }
    else if (fp.fields[i].field->serializer.decode == nullptr) {
      result.KV[fp.fields[i].field->name] = stream.nReadVarUInt32();
      ////std::cout << "Decoded default: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << result.KV[fp.fields[i].field->name] << "\n";
      
      std::cout << "Decoded default: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << asString(result.KV[fp.fields[i].field->name]) << "\n";
      continue;
    }
    else {
      result.KV[fp.fields[i].field->name] = fp.fields[i].field->serializer.decode(stream, fp.fields[i].field);
    }
    
    ////std::cout << "Decoded: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << result.KV[fp.fields[i].field->name] << "\n";
    
    std::cout << "Decoded: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << asString(result.KV[fp.fields[i].field->name]) << "\n";
  }
  
  return result;
}
