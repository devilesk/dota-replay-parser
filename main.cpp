#include "replayviewer.hpp"

ReplayViewer r;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: parser <replay file>" << std::endl;
        return 1;
    }

    std::string path(argv[1]);
    
    r.open(path);
    while (r.good()) {
        r.setReplayTick(r.getReplayTick()+1);
        r.getCurrentTickState();
    }
    
    std::cout << "Last tick: " << std::to_string(r.getReplayTick()) << "\n";
    
    google::protobuf::ShutdownProtobufLibrary();
    
    std::cout << "done" << std::endl;
}