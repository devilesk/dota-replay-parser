#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <snappy.h>
//#include <SDL2/SDL.h>
#include <stdio.h>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// Protobuf objects
#include "protobufs/ai_activity.pb.h"
#include "protobufs/demo.pb.h"
#include "protobufs/dota_commonmessages.pb.h"
#include "protobufs/dota_modifiers.pb.h"
#include "protobufs/usermessages.pb.h"
#include "protobufs/netmessages.pb.h"
#include "protobufs/networkbasetypes.pb.h"
#include "protobufs/usermessages.pb.h"
#include "protobufs/dota_usermessages.pb.h"
#include "protobufs/gameevents.pb.h"

#include "bitstream.hpp"
#include "parser_types.hpp"
#include "fieldpath.hpp"
#include "property.hpp"
#include "property_decoder.hpp"
#include "flattened_serializers.hpp"
#include "class_info.hpp"
#include "string_table.hpp"
#include "message_lookup.hpp"
#include "packet_entity.hpp"

//uint32_t readVarUInt32(std::ifstream &stream);
//uint32_t readVarUInt32(const char*, int &pos);
int packet_priority(int type);
bool compare_packet_priority(const pendingMessage &i, const pendingMessage &j);

inline bool isPrefix(std::string const &s1, std::string const &s2) {
    return s1.length() >= s2.length() && (0 == s1.compare(0, s2.length(), s2, 0, s2.length()));
}

void entityHandler(PacketEntity* pe, EntityEventType t);

#endif /* _PARSER_HPP_ */
