#include "flattened_serializers.hpp"

// NOT FULLY IMPLEMENTED
std::shared_ptr<dt> recurseTable(flattened_serializers* sers, CSVCMsg_FlattenedSerializer* msg, const ProtoFlattenedSerializer_t* serializer) {
  auto datatable = std::make_shared<dt>(dt {});
  datatable->name = msg->symbols(serializer->serializer_name_sym());
  datatable->version = serializer->serializer_version();
  datatable->properties = std::vector<std::shared_ptr<dt_property>>();
  for (int i = 0; i < serializer->fields_index_size(); ++i) {
    const ProtoFlattenedSerializerField_t &pField = msg->fields(serializer->fields_index(i));
    auto prop = std::make_shared<dt_property>(dt_property {
      nullptr,
      nullptr
    });
    prop->field = new dt_field {
      msg->symbols(pField.var_name_sym()),
      "",
      msg->symbols(pField.var_type_sym()),
      -1,
      pField.encode_flags(),
      pField.has_encode_flags(),
      pField.bit_count(),
      pField.has_bit_count(),
      pField.low_value(),
      pField.has_low_value(),
      pField.high_value(),
      pField.has_high_value(),
      pField.field_serializer_version(),
      nullptr,
      sers->build
    };

    // Fill the serializer
    //sers.pst.FillSerializer(prop.Field)
    fillSerializer(sers->pst, prop->field);
    
    // Optional: Attach encoder
    if (pField.has_var_encoder_sym()) {
      prop->field->encoder = msg->symbols(pField.var_encoder_sym());
    }
    else {
      // TODO:
      
    }

    // Optional: Attach the serializer version for the property if applicable
    if (pField.has_field_serializer_name_sym()) {
      prop->table = sers->serializers[msg->symbols(pField.field_serializer_name_sym())][pField.field_serializer_version()];
    }

    // Optional: Adjust array fields
    if (prop->field->serializer->isArray) {
      auto tmpDt = std::make_shared<dt>(dt {
        prop->field->name,
        -1,
        0,
        std::vector<std::shared_ptr<dt_property>>()
      });
      //std::cout << "isArray prop.field.name: " << prop.field.name << "\n";
      
      // Add each array field to the table
      for (int i = 0; i < (int)prop->field->serializer->length; ++i) {
        char buf[5];
        sprintf(buf, "%04d", i);
        std::string n = std::string(buf, 4);
        //std::cout << "n: " << n << "\n";
        
        tmpDt->properties.push_back(std::make_shared<dt_property>(dt_property {
          new dt_field {
            n,
            prop->field->encoder,
            prop->field->serializer->name,
            i,
            prop->field->flags,
            prop->field->has_flags,
            prop->field->bit_count,
            prop->field->has_bit_count,
            prop->field->low_value,
            prop->field->has_low_value,
            prop->field->high_value,
            prop->field->has_high_value,
            prop->field->version,
            prop->field->serializer->arraySerializer,
            prop->field->build
          },
          prop->table // This carries on the actual table instead of overriding it
        }));
        
        // Copy parent prop to rename it's name according to the array index
        if (prop->table != nullptr) {
          //std::cout << "prop->table != nullptr\n";
          //std::cout << "prop->table->name" << prop->table->name << "\n";
          auto nTable = std::make_shared<dt>(dt {});
          *nTable = *prop->table;
          char buf2[5];
          sprintf(buf2, "%04d", i);
          std::string n = std::string(buf2, 4);
          nTable->name = n;
          //std::cout << "nTable->name: " << n << "\n";
          tmpDt->properties.back()->table = nTable;
          //std::cout << "tmpDt->properties[tmpDt->properties.size() - 1]->table->name" << tmpDt->properties[tmpDt->properties.size() - 1]->table->name << "\n";
        }
      }
      prop->table = tmpDt;
    }
    datatable->properties.push_back(prop);
  }
  return datatable;
}

flattened_serializers Parser::parseSendTables(CDemoSendTables* sendTables, PropertySerializerTable* pst) {
  //std::cout << "parsing DEM_SendTables\n";
  uint32_t size;
  const std::string &data = sendTables->data();
  int pos = 0;
  
  size = readVarUInt32(data.c_str(), pos);
  //dota::bitstream stream(data);
  //size = stream.nReadVarUInt32();
  
  //std::cout << "data length: " << data.length() << "\n";
  //std::cout << "size: " << std::to_string(size) << " pos: " << std::to_string(pos) << "\n";
  CSVCMsg_FlattenedSerializer msg;
  msg.ParseFromArray(&data[pos], size);
  //std::cout << "# serializers: " << std::to_string(msg.serializers_size()) << "\n";
  
  //std::unordered_map< std::string, std::unordered_map<int, dt> > serializers;
  flattened_serializers fs = {
    std::unordered_map< std::string, std::unordered_map<int, std::shared_ptr<dt>> >(),
    pst,
    GameBuild
  };
  //fs.proto = &msg;
  //fs.build = ;
  
  for (int i = 0; i < msg.serializers_size(); ++i) {
    const ProtoFlattenedSerializer_t &serializer = msg.serializers(i);
    std::string sName = msg.symbols(serializer.serializer_name_sym());
    int sVer = serializer.serializer_version();
    if (fs.serializers.find(sName) == fs.serializers.end()) {
      fs.serializers[sName] = std::unordered_map<int, std::shared_ptr<dt>>();
    }
    //std::cout << "add table named to fs.serializers: " << datatable.name << "\n";
    fs.serializers[sName][sVer] = recurseTable(&fs, &msg, &serializer);
  
    //std::cout << "serializer index: " << std::to_string(i) << "\n";
    //std::cout << "serializer_name_sym: " << std::to_string(serializer.serializer_name_sym()) << ", " << msg.symbols(serializer.serializer_name_sym()) << "\n";
    //std::cout << "serializer_version: " << std::to_string(serializer.serializer_version()) << "\n";
    //std::cout << "fields_index_size: " << std::to_string(serializer.fields_index_size()) << "\n";
    //std::cout << "datatable properties size: " << std::to_string(datatable.properties.size()) << "\n";
    /*for (int j = 0; j < serializer.fields_index_size(); ++j) {
      //std::cout << "field_index: " << std::to_string(serializer.fields_index(j)) << "\n";
      ProtoFlattenedSerializerField_t field = msg.fields(serializer.fields_index(j));
      //std::cout << "var_name_sym: " << std::to_string(field.var_name_sym()) << ", " << msg.symbols(field.var_name_sym()) << "\n";
    }*/
  }
  //std::cout << "symbols_size: " << std::to_string(msg.symbols_size()) << "\n";
  //std::cout << "fields_size: " << std::to_string(msg.fields_size()) << "\n";
  //serializers = fs.serializers;
  ////std::cout << "serializers: " << std::to_string(serializers["CWorld"][0].properties.size()) << "\n";
  //return 0;
  return fs;
}

void Parser::onCDemoSendTables(const char* buffer, int size) {
  CDemoSendTables data;
  data.ParseFromArray(buffer, size);
  serializers = parseSendTables(&data, getDefaultPropertySerializerTable()).serializers;
}