#include <server.h>
#include <iostream>
#include <stdexcept>


enum ServerCmdArgs {
    PORT = 1,
    COUNT           // number of cmd arguments for server
};

int main(int argc, char **argv) {
    if (argc != ServerCmdArgs::COUNT) {
        std::cout << "Wrong number of arguments!" << std::endl;
        exit(1);
    }

    Server server(std::stoul(argv[ServerCmdArgs::PORT]));

    try {
        server.runServerForever();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
