#ifndef _STRING_TABLE_HPP_
#define _STRING_TABLE_HPP_

#include <iostream>
#include <snappy.h>

#include "parser_types.hpp"
#include "class_info.hpp"

#include "protobufs/netmessages.pb.h"

uint32_t onCSVCMsg_CreateStringTable(CSVCMsg_CreateStringTable* data, parser* p);
std::vector<StringTableItem> parseStringTable(const char* buffer, int size, int num_updates, bool userDataFixed, int userDataSize);

#endif /* _STRING_TABLE_HPP_ */