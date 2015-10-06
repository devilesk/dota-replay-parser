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
    //std::cout << "Decoding field: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << fp.fields[i].field->encoder << "\n";
    if (fp.fields[i].field->serializer.decodeContainer != nullptr) {
      //std::cout << "Decoding container: " << fp.fields[i].field->name << "\n";
      result.KV[fp.fields[i].field->name] = fp.fields[i].field->serializer.decodeContainer(stream, fp.fields[i].field);
    }
    else if (fp.fields[i].field->serializer.decode == nullptr) {
      result.KV[fp.fields[i].field->name] = stream.nReadVarUInt32();
      //////std::cout << "Decoded default: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << result.KV[fp.fields[i].field->name] << "\n";
      
      //std::cout << "Decoded default: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << asString(result.KV[fp.fields[i].field->name]) << "\n";
      continue;
    }
    else {
      result.KV[fp.fields[i].field->name] = fp.fields[i].field->serializer.decode(stream, fp.fields[i].field);
    }
    
    //////std::cout << "Decoded: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << result.KV[fp.fields[i].field->name] << "\n";
    
    //std::cout << "Decoded: " << stream.position() << " " << fp.fields[i].field->name << " " << fp.fields[i].field->type << " " << asString(result.KV[fp.fields[i].field->name]) << "\n";
  }
  
  return result;
}

void Properties::merge(Properties* p2) {
  for(std::map<std::string, value_type>::iterator iter = p2->KV.begin(); iter != p2->KV.end(); ++iter)
  {
    //std::string k =  iter->first;
    //value_type v = iter->second;
    KV[iter->first] = iter->second;
  }
}
bool Properties::fetch(std::string k, value_type& v) {
  if (KV.find(k) != KV.end()) {
    v = KV[k];
    return true;
  }
  return false;
}
bool Properties::fetchBool(std::string k, bool& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<bool>(KV[k]);
    return true;
  }
  v = false;
  return false;
}
bool Properties::fetchInt32(std::string k, int32_t& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<int32_t>(KV[k]);
    return true;
  }
  v = 0;
  return false;
}
bool Properties::fetchUint32(std::string k, uint32_t& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<uint32_t>(KV[k]);
    return true;
  }
  v = 0;
  return false;
}
bool Properties::fetchUint64(std::string k, uint64_t& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<uint64_t>(KV[k]);
    return true;
  }
  v = 0;
  return false;
}
bool Properties::fetchFloat32(std::string k, float& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<float>(KV[k]);
    return true;
  }
  v = 0.0;
  return false;
}
bool Properties::fetchString(std::string k, std::string& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<std::string>(KV[k]);
    return true;
  }
  v = "";
  return false;
}
