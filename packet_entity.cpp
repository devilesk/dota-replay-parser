#include "packet_entity.hpp"

bool PacketEntity::fetch(const std::string &k, value_type& v) {
  if (properties->fetch(k, v)) return true;
  return classBaseline->fetch(k, v);
}
bool PacketEntity::fetchBool(const std::string &k, bool& v) {
  if (properties->fetchBool(k, v)) return true;
  return classBaseline->fetchBool(k, v);
}
bool PacketEntity::fetchInt32(const std::string &k, int32_t& v) {
  if (properties->fetchInt32(k, v)) return true;
  return classBaseline->fetchInt32(k, v);
}
bool PacketEntity::fetchUint32(const std::string &k, uint32_t& v) {
  if (properties->fetchUint32(k, v)) return true;
  return classBaseline->fetchUint32(k, v);
}
bool PacketEntity::fetchUint64(const std::string &k, uint64_t& v) {
  if (properties->fetchUint64(k, v)) return true;
  return classBaseline->fetchUint64(k, v);
}
bool PacketEntity::fetchFloat32(const std::string &k, float& v) {
  if (properties->fetchFloat32(k, v)) return true;
  return classBaseline->fetchFloat32(k, v);
}
bool PacketEntity::fetchString(const std::string &k, std::string& v) {
  if (properties->fetchString(k, v)) return true;
  return classBaseline->fetchString(k, v);
}

void Parser::onCSVCMsg_PacketEntities(const char* buffer, int size) {
  // Skip processing if we're configured not to.
  if (!processPacketEntities) return;
  
  CSVCMsg_PacketEntities data;
  data.ParseFromArray(buffer, size);
  
  //std::cout << "pTick=" << std::to_string(tick) << " isDelta=" << std::to_string(data.is_delta()) << " deltaFrom=" << std::to_string(data.delta_from()) << " updateEntries=" << std::to_string(data.updated_entries()) << " maxEntries=" << std::to_string(data.max_entries()) << " baseline=" << std::to_string(data.baseline()) << " updateBaseline=" << std::to_string(data.update_baseline()) << "\n";
  
  // Skip processing full updates after the first. We'll process deltas instead.
  if (!data.is_delta() && packetEntityFullPackets > 0) return;
  
  // Updates pending
  std::vector<packetEntityUpdate> updates;
  
  // deleted entities
  std::vector<PacketEntity*> deletes;
  
  dota::bitstream stream(data.entity_data());
  int index = -1;
  PacketEntity* pe = nullptr;
  bool ok = false;
  
  // Iterate over all entries
  for (int i = 0; i < (int)(data.updated_entries()); ++i) {
    // Read the index delta from the buffer. This is an implementation
		// from Alice. An alternate implementation from Yasha has the same result.
    uint32_t delta = stream.nReadUBitVar();
    index += (int)(delta) + 1;
    
    //std::cout << "index delta is " << std::to_string(delta) << " to " << std::to_string(index) << "\n";
    
		// Read the type of update based on two booleans.
		// This appears to be backwards from source 1:
		// true+true used to be "create", now appears to be false+true?
		// This seems suspcious.
    EntityEventType eventType = EntityEventType_None;
    if (stream.read(1) != 0) {
      if (stream.read(1) != 0) {
        eventType = EntityEventType_Delete;
      }
      else {
        eventType = EntityEventType_Leave;
      }
    }
    else {
      if (stream.read(1) != 0) {
        eventType = EntityEventType_Create;
      }
      else {
        eventType = EntityEventType_Update;
      }
    }
    
    //std::cout << "update type is " << std::to_string(eventType) << " to " << std::to_string(index) << "\n";
    
    Properties* props;
    
    // Proceed based on the update type
    switch(eventType) {
      case EntityEventType_Create:
        // Create a new PacketEntity.
        pe = new PacketEntity();
        pe->index = index;
        pe->classId = (int)(stream.read(classIdSize));
        pe->properties = new Properties();
        pe->serial = (int)(stream.read(17));
        //std::cout << "classIdSize " << std::to_string(classIdSize) << "\n";
        
        // We don't know what this is used for.
        stream.nReadVarUInt32();
        
        // Get the associated class
        if (classInfo.find(pe->classId) != classInfo.end()) {
          pe->className = classInfo[pe->classId];
        }
        else {
          std::cout << "unable to find class " << std::to_string(pe->classId) << "\n";
        }
        
        // Get the associated baseline
        if (classBaselines.find(pe->classId) != classBaselines.end()) {
          pe->classBaseline = classBaselines[pe->classId];
        }
        else {
          std::cout << "unable to find class baseline " << std::to_string(pe->classId) << "\n";
        }
        
        // Get the associated serializer
        if (serializers.find(pe->className) != serializers.end()) {
          pe->flatTbl = serializers[pe->className][0];
        }
        else {
          //std::cout << "unable to find serializer for class " << pe->className << "\n";
        }
        
        // Register the packetEntity with the parser.
        packetEntities[index] = pe;
        
        // Read properties
        props = readProperties(stream, pe->flatTbl);
        pe->properties->merge(props);
        delete props;
        break;
      case EntityEventType_Update:
        // Find the existing packetEntity
        if (packetEntities.find(index) != packetEntities.end()) {
          pe = packetEntities[index];
        }
        else {
          //std::cout << "unable to find packet entity " << std::to_string(index) << " for update\n";
        }
        
        // Read properties and update the packetEntity
        props = readProperties(stream, pe->flatTbl);
        pe->properties->merge(props);
        delete props;
        break;
      case EntityEventType_Delete:
        /*if (packetEntities.find(index) == packetEntities.end()) {
          std::cout << "unable to find packet entity " << std::to_string(index) << " for delete\n";
        }
        std::cout << "deleting packet entity " << std::to_string(index) << "\n";*/
        if (packetEntities.find(index) != packetEntities.end()) {
          pe = packetEntities[index];
        }
        deletes.push_back(pe);
        //delete packetEntities[index];
        packetEntities.erase(index);
        
        break;
      case EntityEventType_Leave:
        // TODO: Decide how we want to handle this
        break;
      case EntityEventType_None:
        break;
    }
    
    // Add the update to the list of pending updates.
    //std::cout << "adding packet entity update" << std::to_string(index) << " for " << std::to_string(eventType) << "\n";
    packetEntityUpdate pu {
      pe,
      eventType
    };
    updates.push_back(std::move(pu));
  }
  
  // Update the full packet count.
  if (!data.is_delta()) {
    ++packetEntityFullPackets;
  }
  
	// Offer all packet entity updates to callback handlers. This is done
	// only after all updates have been processed to ensure consistent state.
  for(std::vector<packetEntityUpdate>::iterator it = updates.begin(); it != updates.end(); ++it) {
    for(std::vector<packetEntityHandler>::iterator fn = packetEntityHandlers.begin(); fn != packetEntityHandlers.end(); ++fn) {
        (*fn)(it->pe, it->t);
    }
  }
  
  // free deleted entities
  for (auto & p : deletes) {
    delete p->properties;
    delete p;
  }
}