#pragma once

#include <string>
#include <cstdint>
#include <netinet/in.h>

class Client {
public:
    Client(const std::string& host, uint16_t port, const std::string& name, uint32_t period);
    Client(uint16_t port, const std::string& name, uint32_t period);
    void runClientForever();

private:
    bool connectToServer();
    bool closeConnection();
    std::string getMessageInFormat() const;
    bool sendToServer(const std::string& message) const;

private:
    std::string name_{};
    std::string host_{};
    uint16_t port_;
    uint32_t period_;
    int sock_fd_;

    const int DOMAIN_ = AF_INET;
    const int CONN_TYPE_ = SOCK_STREAM;
    const uint8_t CONN_RETRY_ = 3;
    const uint8_t CONN_RETRY_PERIOD_SECONDS_ = 3;
    const std::string FORMAT_ = "[%04d-%02d-%02d %02d:%02d:%02d.%03d] %s";
};
