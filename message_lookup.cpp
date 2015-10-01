#include "message_lookup.hpp"

uint32_t parsePendingMessage(pendingMessage* msg, parser* p) {
  switch(msg->type) {
    case SVC_Messages::svc_CreateStringTable: {
      std::cout << "svc_CreateStringTable\n";
      CSVCMsg_CreateStringTable data;
      data.ParseFromString(msg->data);
      onCSVCMsg_CreateStringTable(&data, p);
    }
    break;
    case SVC_Messages::svc_ServerInfo: {
      std::cout << "svc_ServerInfo\n";
      CSVCMsg_ServerInfo data;
      data.ParseFromString(msg->data);
      onCSVCMsg_ServerInfo(&data, p);
    }
    break;
  }
  std::cout << "type: " << std::to_string(msg->type) << "\n";
  return 0;
}