#include "message_lookup.hpp"

std::unordered_map<int, Callback> handlers = initHandlers();

std::unordered_map<int, Callback> initHandlers() {
  std::unordered_map<int, Callback> handlers;
  handlers[SVC_Messages::svc_CreateStringTable] = &Parser::onCSVCMsg_CreateStringTable;
  handlers[SVC_Messages::svc_PacketEntities] = &Parser::onCSVCMsg_PacketEntities;
  handlers[SVC_Messages::svc_UpdateStringTable] = &Parser::onCSVCMsg_UpdateStringTable;
  handlers[SVC_Messages::svc_ServerInfo] = &Parser::onCSVCMsg_ServerInfo;
  return handlers;
}

uint32_t Parser::parsePendingMessage(pendingMessage* msg) {
  /*switch(msg->type) {
    case SVC_Messages::svc_CreateStringTable: {
      //std::cout << "svc_CreateStringTable\n";
      CSVCMsg_CreateStringTable data;
      data.ParseFromString(msg->data);
      onCSVCMsg_CreateStringTable(&data);
    }
    break;
    case SVC_Messages::svc_ServerInfo: {
      //std::cout << "svc_ServerInfo\n";
      CSVCMsg_ServerInfo data;
      data.ParseFromString(msg->data);
      onCSVCMsg_ServerInfo(&data);
    }
    break;
  }*/
  //handlers[msg->type](msg->data);
  //std::cout << "pendingMessage type: " << std::to_string(msg->type) << "\n";
  if (handlers.find(msg->type) != handlers.end()) (this->*handlers[msg->type])(msg->data);
  return 0;
}