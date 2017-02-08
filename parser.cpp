#include "parser.hpp"

const std::string Parser::demheader = "PBDEMS2";

void Parser::open(std::string path) {
    stream.open(path.c_str(), std::ifstream::in | std::ifstream::binary);
    if (!stream.is_open()) {
        std::cout << "could not open file" << path << "\n";
    }
    else {
        std::cout << "file " << path << " opened\n";
    }

    // get file length
    stream.seekg(0, stream.end);
    length = stream.tellg();
    std::cout << "file length: " << std::to_string(length) << "\n";
    stream.seekg(0, stream.beg);
    
    buffer = new char[length];
    
    stream.read(buffer, length);
    stream.close();
    
    pos = 0;
}

void Parser::readHeader() {
    char header[8];

    std::copy(buffer, &buffer[7], header);
    pos += 8;
    
    if (strcmp(header, demheader.c_str())) {
        std::cout << "invalid header\n";
    }
    else {
        std::cout << "valid header\n";
    }

    // Skip the next 8 bytes, which appear to be two int32s related to the size
    // of the demo file. We may need them in the future, but not so far.
    pos += 8;

    //uint32_t cmd;
    //cmd = readVarUInt32(&buffer[pos], pos);
    readVarUInt32(&buffer[pos], pos);

    tick = readVarUInt32(&buffer[pos], pos);

    // This appears to actually be an int32, where a -1 means pre-game.
    if (tick == 4294967295) {
        tick = 0;
    }
    
    uint32_t size;
    size = readVarUInt32(&buffer[pos], pos);

    CDemoFileHeader demo_header;
    demo_header.ParseFromArray(&buffer[pos], size);
    std::cout << "map_name: " << demo_header.map_name() << "\n";

    pos += size;
}

void Parser::handle() {
    while (good()) {
        read();
    }
}

void Parser::generateFullPacketCache() {
    fpackcache.clear();
    fpackcachetick.clear();
    std::cout << "generate the cache\n";
    uint32_t tmpPos = pos;
    pos = 0;
    readHeader();
    uint32_t cmd;
    uint32_t t;
    uint32_t size;
    //bool compressed;
    uint32_t p;
    
    do {
        p = pos;
        
        cmd = readVarUInt32(&buffer[pos], pos);
        cmd = (cmd & ~EDemoCommands::DEM_IsCompressed);
        t = readVarUInt32(&buffer[pos], pos);
        size = readVarUInt32(&buffer[pos], pos);
        
        pos += size;
        
        if (cmd == 13) {
            fpackcache.push_back(p);
            fpackcachetick.push_back(t);
        }
    } while (cmd != 0);
    
    stopTick = t;
    pos = tmpPos;
    std::cout << "cache generated, stopTick: " << std::to_string(stopTick) << "\n";
}

void Parser::skipTo(uint32_t _tick) {
    std::cout << "skipTo " << std::to_string(_tick) << "\n";
    // make sure we have a valid state before skipping ahead / back
    while (!syncTick) {
        read();
    }
    
    // skip to closest fullpacket
    seekToFullPacket(_tick);
    
    // read until at desired tick
    std::cout << "read until at desired tick" << std::to_string(tick) << " " << std::to_string(_tick) << "\n";
    while (tick < _tick) {
        read();
    }
}

void Parser::seekToFullPacket(int _tick) {
    std::cout << "seekToFullPacket\n";
    // generate the cache
    if (fpackcache.empty()) {
        generateFullPacketCache();
    }

    // seek to the fullpacket closest to desired tick
    for (int i = 0; (uint32_t)i < fpackcachetick.size(); ++i) {
        if (fpackcachetick[i] >= (uint32_t)_tick) {
            break;
        }
        
        // here we parse all full packets up to requested tick
        // it seems like all full packet string tables need to be parsed to get a consistent state
        // but only entities in the last full packet need to be processed
        // for now we fully parse all of them anyway with read()
        
        // clear all entities
        packetEntities.clear();
    
        // parse full packet
        pos = fpackcache[i];
        packetEntityFullPackets = 0;
        read();
    }
}

void Parser::readMessageHeader(const char* buffer, int &pos, uint32_t& cmd, uint32_t& _tick, uint32_t& size, bool& compressed) {
    cmd = readVarUInt32(&buffer[pos], pos);
    compressed = (cmd & EDemoCommands::DEM_IsCompressed) == EDemoCommands::DEM_IsCompressed;
    cmd = (cmd & ~EDemoCommands::DEM_IsCompressed);

    _tick = readVarUInt32(&buffer[pos], pos);
    
    // This appears to actually be an int32, where a -1 means pre-game.
    if (_tick == 4294967295) {
        _tick = 0;
    }

    /*if (tick != _tick) {
        std::cout << "tick: " << std::to_string(_tick) << "\n";
    }*/
    
    size = readVarUInt32(&buffer[pos], pos);
}

void Parser::readMessage(const char* buffer, int &pos) {
    uint32_t cmd;
    uint32_t _tick;
    uint32_t size;
    bool compressed;

    readMessageHeader(buffer, pos, cmd, _tick, size, compressed);
    
    if (cmd == 3) {
        syncTick = true;
    }
    
    //if (compressed && snappy::IsValidCompressedBuffer(&buffer[pos], size)) {
    if (compressed) {
        //std::cout << "valid snappy compressed buffer\n";
        std::size_t uSize;
        if (snappy::GetUncompressedLength(&buffer[pos], size, &uSize)) {
            char * uBuffer = new char[uSize];
            if (snappy::RawUncompress(&buffer[pos], size, uBuffer)) {
                //std::cout << "uncompressed success, size: " << std::to_string(uSize) << "\n";
                parseMessage(cmd, _tick, uSize, uBuffer);
            }
            else {
                //std::cout << "uncompress fail\n";
                exit(0);
            }
            delete[] uBuffer;
        }
        else {
            //std::cout << "get uncompressed length fail\n";
            exit(0);
        }
    }
    else {
        parseMessage(cmd, _tick, size, &buffer[pos]);
    }
    
    pos += size;
}

void Parser::parseMessage(int cmd, int _tick, int size, const char* buffer) {
    tick = _tick;
    CDemoPacket packet;
    switch(cmd) {
        case 4:
            //std::cout << "type: DEM_SendTables\n";
            onCDemoSendTables(buffer, size);
        break;
        case 5:
            //std::cout << "type: DEM_ClassInfo\n";
            onCDemoClassInfo(buffer, size);
        break;
        case 6:
            //std::cout << "type: DEM_StringTables\n";
            //CDemoStringTables stringTables;
            //stringTables.ParseFromArray(buffer, size);
        break;
        case 7:
            //std::cout << "type: DEM_Packet\n";
        case 8:
            //std::cout << "type: DEM_SignonPacket\n";
            packet.ParseFromArray(buffer, size);
            onCDemoPacket(&packet);
        break;
        case 13:
            //std::cout << "type: DEM_FullPacket\n";
            CDemoFullPacket packet;
            packet.ParseFromArray(buffer, size);
            onCDemoFullPacket(&packet);
        break;
    }
}

uint32_t Parser::parseStringTables(const CDemoStringTables* stringTables) {
    for (int i = 0; i < stringTables->tables_size(); ++i) {
        CDemoStringTables_table_t t = stringTables->tables(i);
        //std::cout << "table_name: " << t.table_name() << "\n";
        for (int j = 0; j < t.items_size(); ++j) {
            CDemoStringTables_items_t item = t.items(j);
            //std::cout << "str: " << item.str() << "\n";
            uint32_t size;
            int pos = 0;
            size = readVarUInt32(item.data().c_str(), pos);
            //std::cout << "data size: " << std::to_string(size) << "\n";
            if (size > 4) {
                //std::cout << "data: ";
                for (int k = 0; k < 4; ++k) {
                    //std::cout << item.data().c_str()[pos + k];
                }
                //std::cout << "\n";
                do
                {
                    //std::cout << "Press a key to continue...\n";
                } while (std::cin.get() != '\n');
            }
        }
        for (int j = 0; j < t.items_clientside_size(); ++j) {
            CDemoStringTables_items_t item = t.items_clientside(j);
            //std::cout << "str: " << item.str() << "\n";
        }
        //std::cout << "items_size: " << std::to_string(t.items_size()) << "\n";
        //std::cout << "items_clientside_size: " << std::to_string(t.items_clientside_size()) << "\n";
        //std::cout << "table_flags: " << std::to_string(t.table_flags()) << "\n";
        do
        {
            //std::cout << "Press a key to continue...\n";
        } while (std::cin.get() != '\n');
    }
    return 0;
}

int packet_priority(int type) {
    switch(type) {
        case NET_Messages::net_Tick:
        case SVC_Messages::svc_CreateStringTable:
        case SVC_Messages::svc_UpdateStringTable:
        case NET_Messages::net_SpawnGroup_Load:
            return -10;
        break;
        case SVC_Messages::svc_PacketEntities:
            return 5;
        break;
        case EBaseGameEvents::GE_Source1LegacyGameEvent:
            return 10;
        break;
    }
    return 0;
}

bool compare_packet_priority(const pendingMessage &i, const pendingMessage &j) {
    if (i.tick > j.tick) return false;
    if (i.tick < j.tick) return true;
    return packet_priority(i.type) < packet_priority(j.type);
}

void Parser::onCDemoPacket(const CDemoPacket* packet) {
    dota::bitstream stream(packet->data());
    int type;
    uint32_t size;
    std::vector<pendingMessage> pendingMessages;
    while (stream.end() - stream.position() >= 8) {
        type = stream.nReadUBitVar();
        size = stream.nReadVarUInt32();
        ////std::cout << "type: " << std::to_string(type) << "\n";
        ////std::cout << "size: " << std::to_string(size) << "\n";
        char * buffer = new char[8 * size];
        stream.readBits(buffer, 8 * size);
        pendingMessage msg;
        msg.type = type;
        msg.tick = tick;
        msg.data = std::string(&buffer[0], size);
        delete[] buffer;
        pendingMessages.push_back(std::move(msg));
    }
    std::stable_sort(pendingMessages.begin(), pendingMessages.end(), compare_packet_priority);
    for (std::vector<pendingMessage>::iterator it = pendingMessages.begin(); it != pendingMessages.end(); ++it) {
        ////std::cout << "priority: " << std::to_string(packet_priority(it->type)) << "\n";
        //std::cout << "TICK: " << std::to_string(tick) << "\n";
        parsePendingMessage(&*it);
    }
}

void Parser::onCDemoFullPacket(const CDemoFullPacket* packet) {
    //parseStringTables(&(packet->string_table()), p);
    onCDemoStringTables(&(packet->string_table()));
    onCDemoPacket(&(packet->packet()));
}

void Parser::onCUserMessageSayText2(const char* buffer, int size) {
    CUserMessageSayText2 data;
    data.ParseFromArray(buffer, size);
    std::cout << data.messagename() << " | " << data.param1() << ": " << data.param2() << "\n";
}
