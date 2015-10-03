#include "flattened_serializers.hpp"

// NOT FULLY IMPLEMENTED
int recurseTable(flattened_serializers* sers, dt* datatable, CSVCMsg_FlattenedSerializer* msg, ProtoFlattenedSerializer_t* serializer) {
  datatable->name = msg->symbols(serializer->serializer_name_sym());
  datatable->version = serializer->serializer_version();
  datatable->properties = std::vector<dt_property>();
  for (int i = 0; i < serializer->fields_index_size(); ++i) {
    ProtoFlattenedSerializerField_t pField = msg->fields(serializer->fields_index(i));
    dt_property prop;
    dt_field propField;
    prop.table = nullptr;
    prop.field = propField;
    prop.field.name = msg->symbols(pField.var_name_sym());
    prop.field.index = -1;
    prop.field.flags = pField.encode_flags();
    prop.field.has_flags = pField.has_encode_flags();
    prop.field.bit_count = pField.bit_count();
    prop.field.has_bit_count = pField.has_bit_count();
    prop.field.low_value = pField.low_value();
    prop.field.has_low_value = pField.has_low_value();
    prop.field.high_value = pField.high_value();
    prop.field.has_high_value = pField.has_high_value();
    prop.field.type = msg->symbols(pField.var_type_sym());
    prop.field.version = pField.field_serializer_version();
    prop.field.build = sers->build;

    // Fill the serializer
    //sers.pst.FillSerializer(prop.Field)
    fillSerializer(&(sers->pst), &(prop.field));
    
    if (pField.has_var_encoder_sym()) {
      prop.field.encoder = sers->proto->symbols(pField.var_encoder_sym());
    }
    else {
      // TODO:

    }

    if (pField.has_field_serializer_name_sym()) {
      std::string pFieldName = sers->proto->symbols(pField.field_serializer_name_sym());
      int pFieldVersion = pField.field_serializer_version();
      dt* pSerializer = new dt();
      *pSerializer = sers->serializers[pFieldName][pFieldVersion];
      prop.table = pSerializer;
    }

    if (prop.field.serializer.isArray) {
      dt tmpDt = {
        prop.field.name,
        0,
        0,
        std::vector<dt_property>()
      };
      std::cout << "isArray prop.field.name: " << prop.field.name << "\n";
      for (int i = 0; i < prop.field.serializer.length; ++i) {
        char buf[4];
        sprintf(buf, "%04d", i);
        std::string n = std::string(buf, 4);
        std::cout << "n: " << n << "\n";
        dt_property new_prop;
        if (prop.field.serializer.arraySerializer != NULL) {
          new_prop = {
            {
              n,
              prop.field.encoder,
              prop.field.serializer.name,
              i,
              prop.field.flags,
              prop.field.has_flags,
              prop.field.bit_count,
              prop.field.has_bit_count,
              prop.field.low_value,
              prop.field.has_low_value,
              prop.field.high_value,
              prop.field.has_high_value,
              prop.field.version,
              *prop.field.serializer.arraySerializer,
              prop.field.build
            },
            prop.table
          };
          //new_prop.field.serializer = *prop.field.serializer.arraySerializer;
        }
        else {
          new_prop = {
            {
              n,
              prop.field.encoder,
              prop.field.serializer.name,
              i,
              prop.field.flags,
              prop.field.has_flags,
              prop.field.bit_count,
              prop.field.has_bit_count,
              prop.field.low_value,
              prop.field.has_low_value,
              prop.field.high_value,
              prop.field.has_high_value,
              prop.field.version,
              {
                nullptr,
                nullptr,
                0,
                0,
                nullptr,
                ""
              },
              prop.field.build
            },
            prop.table
          };
        }
        tmpDt.properties.push_back(new_prop);
        if (prop.table != nullptr) {
          std::cout << "prop.table != nullptr\n";
          std::cout << "prop.table.name" << prop.table->name << "\n";
          dt* nTable = new dt();
          *nTable = *prop.table;
          char buf2[4];
          sprintf(buf2, "%04d", i);
          std::string n = std::string(buf2, 4);
          nTable->name = n;
          std::cout << "nTable.name: " << n << "\n";
          tmpDt.properties[tmpDt.properties.size() - 1].table = nTable;
          std::cout << "tmpDt.properties[tmpDt.properties.size() - 1].table.name" << tmpDt.properties[tmpDt.properties.size() - 1].table->name << "\n";
        }
      }
      dt* t = new dt();
      *t = tmpDt;
      prop.table = t;
    }
    datatable->properties.push_back(prop);
  }
  return 0;
}

uint32_t Parser::parseSendTables(CDemoSendTables* sendTables, PropertySerializerTable pst) {
  std::cout << "parsing DEM_SendTables\n";
  uint32_t size;
  const std::string &data = sendTables->data();
  int pos = 0;
  uint32_t type;
  
  size = readVarUInt32(data.c_str(), &pos);
  //dota::bitstream stream(data);
  //size = stream.nReadVarUInt32();
  
  std::cout << "data length: " << data.length() << "\n";
  std::cout << "size: " << std::to_string(size) << " pos: " << std::to_string(pos) << "\n";
  CSVCMsg_FlattenedSerializer msg;
  msg.ParseFromArray(&data[pos], size);
  std::cout << "# serializers: " << std::to_string(msg.serializers_size()) << "\n";
  
  //std::map< std::string, std::map<int, dt> > serializers;
  flattened_serializers fs = {
    std::map< std::string, std::map<int, dt> >(),
    &msg,
    pst,
    GameBuild
  };
  //fs.proto = &msg;
  //fs.build = ;
  
  for (int i = 0; i < msg.serializers_size(); ++i) {
    ProtoFlattenedSerializer_t serializer = msg.serializers(i);
    std::string sName = msg.symbols(serializer.serializer_name_sym());
    int sVer = serializer.serializer_version();
    if (fs.serializers.find(sName) == fs.serializers.end()) {
      std::map<int, dt> new_dt_map;
      fs.serializers[sName] = new_dt_map;
    }
    dt datatable;
    recurseTable(&fs, &datatable, &msg, &serializer);
    std::cout << "add table named to fs.serializers: " << datatable.name << "\n";
    fs.serializers[sName][sVer] = datatable;
  
    std::cout << "serializer index: " << std::to_string(i) << "\n";
    std::cout << "serializer_name_sym: " << std::to_string(serializer.serializer_name_sym()) << ", " << msg.symbols(serializer.serializer_name_sym()) << "\n";
    std::cout << "serializer_version: " << std::to_string(serializer.serializer_version()) << "\n";
    std::cout << "fields_index_size: " << std::to_string(serializer.fields_index_size()) << "\n";
    std::cout << "datatable properties size: " << std::to_string(datatable.properties.size()) << "\n";
    /*for (int j = 0; j < serializer.fields_index_size(); ++j) {
      std::cout << "field_index: " << std::to_string(serializer.fields_index(j)) << "\n";
      ProtoFlattenedSerializerField_t field = msg.fields(serializer.fields_index(j));
      std::cout << "var_name_sym: " << std::to_string(field.var_name_sym()) << ", " << msg.symbols(field.var_name_sym()) << "\n";
    }*/
  }
  std::cout << "symbols_size: " << std::to_string(msg.symbols_size()) << "\n";
  std::cout << "fields_size: " << std::to_string(msg.fields_size()) << "\n";
  serializers = fs.serializers;
  //std::cout << "serializers: " << std::to_string(serializers["CWorld"][0].properties.size()) << "\n";
  return 0;
}