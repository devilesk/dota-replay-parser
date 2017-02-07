#ifndef _FLATTENED_SERIALIZERS_HPP_
#define _FLATTENED_SERIALIZERS_HPP_

#include "bitstream.hpp"
#include "parser_types.hpp"
#include "property.hpp"
#include "property_serializers.hpp"

#include "protobufs/demo.pb.h"
#include "protobufs/netmessages.pb.h"

std::shared_ptr<dt> recurseTable(flattened_serializers*, CSVCMsg_FlattenedSerializer*, const ProtoFlattenedSerializer_t*);
//uint32_t parseSendTables(CDemoSendTables* sendTables, PropertySerializerTable pst, parser* p);

#endif /* _FLATTENED_SERIALIZERS_HPP_ */