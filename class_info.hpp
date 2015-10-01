#ifndef _CLASS_INFO_HPP_
#define _CLASS_INFO_HPP_

#include <iostream>
#include <math.h>

#include "string_table.hpp"
#include "flattened_serializers.hpp"

#include "protobufs/demo.pb.h"

extern std::regex gameBuildRegexp;

uint32_t onCSVCMsg_ServerInfo(CSVCMsg_ServerInfo* data, parser* p);
uint32_t updateInstanceBaseline(parser* p);
uint32_t updateInstanceBaselineItem(StringTableItem item, parser* p);

#endif /* _CLASS_INFO_HPP_ */