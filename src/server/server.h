#pragma once

#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <mutex>

class Server {
public:
    Server(const std::string& host, uint16_t port);
    Server(uint16_t port);
    void runServerForever();

private:
    bool startListening();
    void processConnection(int conn_fd) const;
    void logInfoFromClient(const char* info) const;

private:
    std::string host_{};
    uint16_t port_;
    int listen_fd_;
    mutable std::mutex log_mtx_;

    const int DOMAIN_ = AF_INET;
    const int CONN_TYPE_ = SOCK_STREAM;
    const uint8_t MAX_CONN_NUM_ = 100;
    const uint16_t MAX_BUF_SIZE_ = 4096;
    const std::string FILENAME_ = "log.txt";
};
