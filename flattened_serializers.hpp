#ifndef _FLATTENED_SERIALIZERS_HPP_
#define _FLATTENED_SERIALIZERS_HPP_

#include "bitstream.hpp"
#include "parser_types.hpp"
#include "property.hpp"
#include "property_serializers.hpp"

#include "protobufs/demo.pb.h"
#include "protobufs/netmessages.pb.h"

// The flattened serializers object
struct flattened_serializers {
  std::map< std::string, std::map<int, dt> > serializers;
  CSVCMsg_FlattenedSerializer* proto;
  PropertySerializerTable pst;
  uint32_t build;
};

int recurseTable(flattened_serializers*, dt*, CSVCMsg_FlattenedSerializer*, ProtoFlattenedSerializer_t*);
uint32_t parseSendTables(CDemoSendTables* sendTables, PropertySerializerTable pst, parser* p);

#endif /* _FLATTENED_SERIALIZERS_HPP_ */