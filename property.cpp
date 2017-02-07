#include "property.hpp"

std::string asString(value_type value) {
    std::string ret;
    propertyToString v(ret);
    boost::apply_visitor( v, value );

    return ret;
}

std::shared_ptr<Properties> readProperties(dota::bitstream &stream, std::shared_ptr<dt> serializer) {  
  // Return type
  std::shared_ptr<Properties> result = std::make_shared<Properties>(Properties {});
  
  // Create fieldpath
  fieldpath fp = newFieldpath(serializer, &huf);
  
  // Get a list of the included fields
  walk(stream, &fp);
  
  // iterate all the fields and set their corresponding values
  for (auto& f: fp.fields) {
    //std::cout << "f->name: " << f->name << "\n";
    //std::cout << "f->field->name: " << f->field->name << "\n";
    //std::cout << "f->field->serializer->name: " << f->field->serializer->name << "\n";
    if (f->field->serializer->decodeContainer != nullptr) {
      result->KV[f->name] = f->field->serializer->decodeContainer(stream, f->field.get());
    }
    else if (f->field->serializer->decode == nullptr) {
      result->KV[f->name] = stream.nReadVarUInt32();
    }
    else {
      result->KV[f->name] = f->field->serializer->decode(stream, f->field.get());
    }
  }
  
  return result;
}

void Properties::merge(Properties* p2) {
  for(std::unordered_map<std::string, value_type>::iterator iter = p2->KV.begin(); iter != p2->KV.end(); ++iter)
  {
    //std::string k =  iter->first;
    //value_type v = iter->second;
    //std::cout << "k: " << iter->first << " v: " << asString(iter->second) << "\n";
    KV[iter->first] = iter->second;
  }
}
bool Properties::fetch(const std::string &k, value_type& v) {
  if (KV.find(k) != KV.end()) {
    v = KV[k];
    return true;
  }
  return false;
}
bool Properties::fetchBool(const std::string &k, bool& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<bool>(KV[k]);
    return true;
  }
  v = false;
  return false;
}
bool Properties::fetchInt32(const std::string &k, int32_t& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<int32_t>(KV[k]);
    return true;
  }
  v = 0;
  return false;
}
bool Properties::fetchUint32(const std::string &k, uint32_t& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<uint32_t>(KV[k]);
    return true;
  }
  v = 0;
  return false;
}
bool Properties::fetchUint64(const std::string &k, uint64_t& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<uint64_t>(KV[k]);
    return true;
  }
  v = 0;
  return false;
}
bool Properties::fetchFloat32(const std::string &k, float& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<float>(KV[k]);
    return true;
  }
  v = 0.0;
  return false;
}
bool Properties::fetchString(const std::string &k, std::string& v) {
  if (KV.find(k) != KV.end()) {
    v = boost::get<std::string>(KV[k]);
    return true;
  }
  v = "";
  return false;
}
