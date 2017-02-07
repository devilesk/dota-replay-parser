#include "parser.hpp"

Parser p;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: parser <replay file>" << std::endl;
        return 1;
    }

    std::string path(argv[1]);
    
    p.open(path);
    p.readHeader();
    p.handle();
    
    std::cout << "Last tick: " << std::to_string(p.tick) << "\n";
    
    google::protobuf::ShutdownProtobufLibrary();
    
    std::cout << "done" << std::endl;
}