#include "property_serializers.hpp"

PropertySerializerTable* getDefaultPropertySerializerTable() {
  PropertySerializerTable* pst = new PropertySerializerTable {
    std::unordered_map< std::string, std::shared_ptr<PropertySerializer>>()
  };
  return pst;
}

std::regex matchArray = std::regex("([^[\\]]+)\\[(\\d+)]");
std::regex matchVector = std::regex("CUtlVector\\<\\s(.*)\\s>$");

void fillSerializer(PropertySerializerTable* pst, dt_field* field) {
  if (field->name.compare("m_flSimulationTime") == 0) {
    field->serializer = std::make_shared<PropertySerializer>(PropertySerializer {
      decodeSimTime,
      nullptr,
      false,
      0,
      nullptr,
      "unkown"
    });
    return;
  }
  else if (field->name.compare("m_flAnimTime") == 0) {
    field->serializer = std::make_shared<PropertySerializer>(PropertySerializer {
      decodeSimTime,
      nullptr,
      false,
      0,
      nullptr,
      "unkown"
    });
    return;
  }
  
  if (field->build < 955) {
    if ((field->name.compare("m_flMana") == 0) || (field->name.compare("m_flMaxMana") == 0)) {
      field->has_low_value = false;
      field->has_high_value = true;
      field->high_value = (float)8192.0;
    }
  }
  
  field->serializer = GetPropertySerializerByName(pst, field->type);
}

std::shared_ptr<PropertySerializer> GetPropertySerializerByName(PropertySerializerTable* pst, const std::string &name) {
  if (pst->serializers.find(name) != pst->serializers.end()) {
    return pst->serializers[name];
  }
  
  //std::cout << "name: " << name << " comp:" << std::to_string(name.compare("uint16") == 0) << "\n";
  // TODO: decoder stuff
  value_type (*decoder)(dota::bitstream &stream, dt_field* f) = nullptr;
  value_type (*decoderContainer)(dota::bitstream &stream, dt_field* f) = nullptr;
  
  if (name.compare("float32") == 0) {
    decoder = decodeFloat;
  }
  else if ((name.compare("int8") == 0) ||
           (name.compare("int16") == 0) ||
           (name.compare("int32") == 0) ||
           (name.compare("int64") == 0)) {
    decoder = decodeSigned;
  }
  else if ((name.compare("uint8") == 0) ||
           (name.compare("uint16") == 0) ||
           (name.compare("uint32") == 0) ||
           (name.compare("uint64") == 0) ||
           (name.compare("Color") == 0)) {
    decoder = decodeUnsigned;
  }
  else if ((name.compare("char") == 0) ||
           (name.compare("CUtlSymbolLarge") == 0)) {
    decoder = decodeString;
  }
  else if (name.compare("Vector") == 0) {
    decoder = decodeFVector;
  }
  else if (name.compare("bool") == 0) {
    decoder = decodeBoolean;
  }
  else if (name.compare("CNetworkedQuantizedFloat") == 0) {
    decoder = decodeQuantized;
  }
  else if ((name.compare("CRenderComponent") == 0) ||
           (name.compare("CPhysicsComponent") == 0) ||
           (name.compare("CBodyComponent") == 0)) {
    decoder = decodeComponent;
  }
  else if (name.compare("QAngle") == 0) {
    decoder = decodeQAngle;
  }
  else if (name.compare("CGameSceneNodeHandle") == 0) {
    decoder = decodeHandle;
  }
  else {
    if (name.compare(0, 7, "CHandle") == 0) {
      decoder = decodeHandle;
    }
    else if (name.compare(0, 13, "CStrongHandle") == 0) {
      decoder = decodeUnsigned;
    }
    else if (name.compare(0, 12, "CUtlVector< ") == 0) {
      std::smatch match;
      if (std::regex_search (name, match, matchVector)) {
        decoderContainer = decodeVector;
        decoder = GetPropertySerializerByName(pst, match[1])->decode;
      }
      else {
        //std::cout << "Unable to read vector type for: " << name << "\n";
      }
    }
    else {
      //std::cout << "no decoder for type: " << name << "\n";
    }
  }
  
  // match all pointers as boolean
  if (name.back() == '*') {
    decoder = decodeBoolean;
  }
  
  std::smatch match;
  if (std::regex_search (name, match, matchArray)) {
    std::string typeName = match[1];
    uint32_t length = std::stoi(match[2], nullptr);
    
    if (pst->serializers.find(typeName) == pst->serializers.end()) {
      pst->serializers[typeName] = GetPropertySerializerByName(pst, typeName);
    }
    
    pst->serializers[name] = std::make_shared<PropertySerializer>(PropertySerializer {
      pst->serializers[typeName]->decode,
      decoderContainer,
      true,
      length,
      pst->serializers[typeName],
      typeName
    });
    
    return pst->serializers[name];
  }
  
  if (std::regex_search (name, match, matchVector)) {
    
    pst->serializers[name] = std::make_shared<PropertySerializer>(PropertySerializer {
      decoder,
      decoderContainer,
      true,
      1024,
      std::make_shared<PropertySerializer>(PropertySerializer {}),
      ""
    });
    
    return pst->serializers[name];
  }
  
  if (name.compare("C_DOTA_ItemStockInfo[MAX_ITEM_STOCKS]") == 0) {
    std::string typeName = "C_DOTA_ItemStockInfo";
    
    if (pst->serializers.find(typeName) == pst->serializers.end()) {
      pst->serializers[typeName] = GetPropertySerializerByName(pst, typeName);
    }
    
    pst->serializers[name] = std::make_shared<PropertySerializer>(PropertySerializer {
      pst->serializers[typeName]->decode,
      decoderContainer,
      true,
      8,
      pst->serializers[typeName],
      typeName
    });
    
    return pst->serializers[name];
  }
  
  if (name.compare("CDOTA_AbilityDraftAbilityState[MAX_ABILITY_DRAFT_ABILITIES]") == 0) {
    std::string typeName = "CDOTA_AbilityDraftAbilityState";
    
    if (pst->serializers.find(typeName) == pst->serializers.end()) {
      pst->serializers[typeName] = GetPropertySerializerByName(pst, typeName);
    }
    
    pst->serializers[name] = std::make_shared<PropertySerializer>(PropertySerializer {
      pst->serializers[typeName]->decode,
      decoderContainer,
      true,
      48,
      pst->serializers[typeName],
      typeName
    });
    
    return pst->serializers[name];
  }
  
  if (name.compare("m_SpeechBubbles") == 0) {
    std::string typeName = "m_SpeechBubbles";
    
    pst->serializers[name] = std::make_shared<PropertySerializer>(PropertySerializer {
      decoder,
      decoderContainer,
      true,
      5,
      nullptr,
      typeName
    });
    
    return pst->serializers[name];
  }
  
  if (name.compare("DOTA_PlayerChallengeInfo") == 0) {
    std::string typeName = "DOTA_PlayerChallengeInfo";
    
    pst->serializers[name] = std::make_shared<PropertySerializer>(PropertySerializer {
      decoder,
      decoderContainer,
      true,
      30,
      nullptr,
      typeName
    });
    
    return pst->serializers[name];
  }

  return std::make_shared<PropertySerializer>(PropertySerializer {
    decoder,
    decoderContainer,
    false,
    0,
    nullptr,
    "unknown"
  });
}