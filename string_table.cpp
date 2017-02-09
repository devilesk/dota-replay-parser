#include "string_table.hpp"

void Parser::onCDemoStringTables(const CDemoStringTables* string_table) {
    //std::cout << "onCDemoStringTables" << std::to_string(tick) << "\n";
    for (auto &tbl : string_table->tables()) {
        //std::cout << "tbl.table_name: " << tbl.table_name() << "\n";
        if (stringTables.nameIndex.find(tbl.table_name()) != stringTables.nameIndex.end() &&
                stringTables.tables.find(stringTables.nameIndex[tbl.table_name()]) != stringTables.tables.end()) {
            auto stringTable = stringTables.tables[stringTables.nameIndex[tbl.table_name()]];

            for (int i = 0; i < tbl.items_size(); ++i) {
                if (stringTable->items.find(i) != stringTable->items.end()) {
                    stringTable->items[i]->key = tbl.items(i).str();
                    stringTable->items[i]->value = tbl.items(i).data();
                }
                else {
                    stringTable->items[i] = std::make_shared<StringTableItem>(StringTableItem {
                        i,
                        tbl.items(i).str(),
                        tbl.items(i).data()
                    });;
                }
            }

            for (int i = 0; i < tbl.items_clientside_size(); ++i) {
                if (stringTable->items.find(i) != stringTable->items.end()) {
                    stringTable->items[i]->key = tbl.items_clientside(i).str();
                    stringTable->items[i]->value = tbl.items_clientside(i).data();
                }
                else {
                    stringTable->items[i] = std::make_shared<StringTableItem>(StringTableItem {
                        i,
                        tbl.items_clientside(i).str(),
                        tbl.items_clientside(i).data()
                    });
                }
            }

            if (stringTable->name.compare("instancebaseline") == 0) {
                updateInstanceBaseline();
                //std::cout << "stringTable->name instancebaseline\n";
            }
        }
        else {
            std::cout << "not found tbl.table_name: " << tbl.table_name() << "\n";
        }
    }
}

void Parser::onCSVCMsg_CreateStringTable(const char* buffer, int size) {
    CSVCMsg_CreateStringTable data;
    data.ParseFromArray(buffer, size);

    //std::cout << "nextIndex: " << std::to_string(stringTables.nextIndex) << "\n";
    //std::cout << "name: " << data.name() << "\n";
    //std::cout << "user_data_fixed_size: " << std::to_string(data.user_data_fixed_size()) << "\n";
    //std::cout << "user_data_size: " << std::to_string(data.user_data_size()) << "\n";
    auto string_table = std::make_shared<StringTable>(StringTable {
        stringTables.nextIndex,
        data.name(),
        std::unordered_map<int, std::shared_ptr<StringTableItem>>(),
        data.user_data_fixed_size(),
        data.user_data_size()
    });

    string_table->items.reserve(512);

    stringTables.nextIndex += 1;
    const std::string &buffer2 = data.string_data();
    //std::cout << "is_compressed: " << std::to_string(data.data_compressed()) << "\n";
    std::vector<std::shared_ptr<StringTableItem>> items;
    if (data.data_compressed()) {
        std::size_t uSize;
        if (snappy::GetUncompressedLength(buffer2.c_str(), buffer2.length(), &uSize)) {
            char * ubuffer2 = new char[uSize];
            if (snappy::RawUncompress(buffer2.c_str(), buffer2.length(), ubuffer2)) {
                //std::cout << "uncompressed success, size: " << std::to_string(uSize) << "\n";
                parseStringTable(ubuffer2, uSize, data.num_entries(), string_table->userDataFixedSize, string_table->userDataSize, items);
            }
            delete[] ubuffer2;
        }
    }
    else {
        parseStringTable(buffer2.c_str(), buffer2.length(), data.num_entries(), string_table->userDataFixedSize, string_table->userDataSize, items);
    }
    for (auto& item: items) {
        string_table->items[item->index] = item;
    }
    stringTables.tables[string_table->index] = string_table;
    stringTables.nameIndex[string_table->name] = string_table->index;
    if (string_table->name.compare("instancebaseline") == 0) {
        updateInstanceBaseline();
    }
}

void Parser::onCSVCMsg_UpdateStringTable(const char* buffer, int size) {
    CSVCMsg_UpdateStringTable data;
    data.ParseFromArray(buffer, size);

    std::shared_ptr<StringTable> t;
    // TODO: integrate
    if (stringTables.tables.find(data.table_id()) != stringTables.tables.end()) {
        t = stringTables.tables[data.table_id()];
    }
    else {
        //std::cout << "missing string table " << std::to_string(data.table_id()) << "\n";
    }

    //std::cout << "tick=" << std::to_string(tick) << " name=" << t->name << " changedEntries=" << std::to_string(data.num_changed_entries()) << " bufflen=" << std::to_string(data.string_data().length()) << "\n";

    std::vector<std::shared_ptr<StringTableItem>> items;
    parseStringTable(data.string_data().c_str(), data.string_data().length(), data.num_changed_entries(), t->userDataFixedSize, t->userDataSize, items);

    for (auto& item: items) {
        int index = item->index;

        if (t->items.find(index) != t->items.end()) {
            // XXX TODO: Sometimes ActiveModifiers change keys, which is suspicous...
            if (item->key.compare("") != 0 && item->key.compare(t->items[index]->key) != 0) {
                //std::cout << "tick=" << std::to_string(tick) << " name=" << t->name << " index=" << std::to_string(index) << " key=" << t->items[index].key << " update key -> " << it->key << "\n";
                t->items[index]->key = item->key;
            }
            if (item->value.length() > 0) {
                //std::cout << "tick=" << std::to_string(tick) << " name=" << t->name << " index=" << std::to_string(index) << " key=" << t->items[index].key << " update value len " << std::to_string(t->items[index].value.length()) << " -> " << std::to_string(it->value.length()) << "\n";
                t->items[index]->value = item->value;
            }
        }
        else {
            //std::cout << "tick=" << std::to_string(tick) << " name=" << t->name << " insert item " << std::to_string(index) << " key " << it->key << "\n";
            t->items[index] = item;
        }
    }

    // Apply the updates to baseline state
    if (t->name.compare("instancebaseline") == 0) {
        updateInstanceBaseline();
    }
}

void parseStringTable(const char* buffer, int buffer_size, int num_updates, bool userDataFixed, int userDataSize, std::vector<std::shared_ptr<StringTableItem>> &items) {
    if (buffer_size == 0) return;
    dota::bitstream stream(std::string(buffer, buffer_size));
    uint32_t index = -1;
    std::vector<std::string> keys;

    // Loop through entries in the data structure
    //
    // Each entry is a tuple consisting of {index, key, value}
    //
    // Index can either be incremented from the previous position or
    // overwritten with a given entry.
    //
    // Key may be omitted (will be represented here as "")
    //
    // Value may be omitted
    for (int i = 0; i < num_updates; ++i) {
        std::string key = "";
        std::string value;

        // Read a boolean to determine whether the operation is an increment or
        // has a fixed index position. A fixed index position of zero should be
        // the last data in the buffer, and indicates that all data has been read.
        uint32_t incr = stream.read(1);
        if (incr) {
            ++index;
        }
        else {
            index = stream.nReadVarUInt32() + 1;
        }
        //std::cout << "index: " << std::to_string(index) << "\n";

        // Some values have keys, some don't.
        bool hasKey = stream.read(1);
        //std::cout << "hasKey: " << std::to_string(hasKey) << "\n";
        if (hasKey) {
            // Some entries use reference a position in the key history for
            // part of the key. If referencing the history, read the position
            // and size from the buffer, then use those to build the string
            // combined with an extra string read (null terminated).
            // Alternatively, just read the string.
            bool useHistory = stream.read(1);
            //std::cout << "useHistory: " << std::to_string(useHistory) << "\n";
            char tmpBuf[1024];
            if (useHistory) {
                uint32_t pos = stream.read(5);
                uint32_t size = stream.read(5);
                stream.nReadString(tmpBuf, 1024);
                if (pos >= keys.size()) {
                    key += std::string(tmpBuf);
                }
                else {
                    std::string s = keys[pos];
                    if (size > s.length()) {
                        key += s + std::string(tmpBuf);
                    }
                    else {
                        key += s.substr(0, size) + std::string(tmpBuf);
                    }
                }
            }
            else {
                stream.nReadString(tmpBuf, 1024);
                key = std::string(tmpBuf);
            }

            if (keys.size() >= 32) {
                keys.erase(keys.begin());
            }
            //std::cout << "key: " << key << "\n";
            keys.push_back(key);
        }

        // Some entries have a value.
        bool hasValue = stream.read(1);
        //std::cout << "hasValue: " << std::to_string(hasValue) << "\n";
        if (hasValue) {
            // Values can be either fixed size (with a size specified in
            // bits during table creation, or have a variable size with
            // a 14-bit prefixed size.
            char * tmpBuf;
            if (userDataFixed) {
                tmpBuf = new char[(int)ceil(userDataSize / 8)];
                stream.readBits(tmpBuf, userDataSize);
                value = std::string(tmpBuf, (int)ceil(userDataSize / 8));
            }
            else {
                uint32_t size = stream.read(14);
                //std::cout << "value size: " << std::to_string(size) << "\n";
                stream.read(3);
                tmpBuf = new char[size];
                stream.readBits(tmpBuf, size * 8);
                value = std::string(tmpBuf, size);
            }
            delete[] tmpBuf;
            //std::cout << "value: " << value << "\n";
        }
        items.push_back(std::make_shared<StringTableItem>(StringTableItem {
            (int)index,
            key,
            value
        }));
    }
}
