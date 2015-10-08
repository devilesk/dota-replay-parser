#ifndef _STRING_TABLE_HPP_
#define _STRING_TABLE_HPP_

#include <iostream>
#include <snappy.h>

#include "parser_types.hpp"
#include "class_info.hpp"

#include "protobufs/netmessages.pb.h"

//uint32_t Parser::onCSVCMsg_CreateStringTable(CSVCMsg_CreateStringTable* data);
void parseStringTable(const char* buffer, int size, int num_updates, bool userDataFixed, int userDataSize, std::vector<StringTableItem> &items);

#endif /* _STRING_TABLE_HPP_ */