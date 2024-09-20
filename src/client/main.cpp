#include <client.h>
#include <stdexcept>
#include <iostream>

enum ClientCmdArgs {
    NAME = 1,
    PORT,
    PERIOD,
    COUNT
};

int main(int argc, char **argv) {
    if (argc != ClientCmdArgs::COUNT) {
        std::cout << "Wrong number of arguments!" << std::endl;
        exit(1);
    }

    Client client(std::stoul(argv[ClientCmdArgs::PORT]),
                  argv[ClientCmdArgs::NAME],
                  std::stoul(argv[ClientCmdArgs::PERIOD]));

    try {
        client.runClientForever();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}