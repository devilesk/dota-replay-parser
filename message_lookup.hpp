#ifndef _MESSAGE_LOOKUP_HPP_
#define _MESSAGE_LOOKUP_HPP_

#include <iostream>

#include "bitstream.hpp"
#include "parser_types.hpp"
#include "string_table.hpp"
#include "class_info.hpp"

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

//uint32_t parsePendingMessage(pendingMessage* msg, parser* p);
extern std::map<int, Callback> handlers;

std::map<int, Callback> initHandlers();

#endif /* _MESSAGE_LOOKUP_HPP_ */