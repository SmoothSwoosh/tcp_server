#include <client.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdexcept>


Client::Client(const std::string& host, uint16_t port, const std::string& name, uint32_t period)
              : host_(host)
              , port_(port)
              , name_(name)
              , period_(period)
{ }

Client::Client(uint16_t port, const std::string& name, uint32_t period)
              : Client::Client("127.0.0.1", port, name, period)
{ }

bool Client::connectToServer() {
    sock_fd_ = socket(DOMAIN_, CONN_TYPE_, 0);
    if (sock_fd_ < 0) {
        return false;
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = DOMAIN_;
    inet_pton(server_addr.sin_family, host_.c_str(), &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(port_);

    if (connect(sock_fd_, (sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        return false;
    }

    return true;
}

bool Client::closeConnection() {
    return !close(sock_fd_);
}

bool Client::sendToServer(const std::string& message) const {
    size_t len = message.size();
    const char* buf = message.c_str();
    ssize_t sent_bytes = 0;                 // number of bytes sent in previous send() call
    size_t total_bytes = 0;                 // total number of sent bytes
    while (total_bytes < len) {
        sent_bytes = send(sock_fd_, buf + total_bytes, len - total_bytes, 0);
        if (sent_bytes < 0) {
            break;
        }
        total_bytes += sent_bytes;
    }
    return sent_bytes >= 0;
}

std::string Client::getMessageInFormat() const {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;
    auto timer = system_clock::to_time_t(now);
    tm* ts = std::localtime(&timer);
    ssize_t message_size = std::snprintf(nullptr, 0, FORMAT_.c_str(),
                                         ts->tm_year + 1900, ts->tm_mon, ts->tm_mday,
                                         ts->tm_hour, ts->tm_min, ts->tm_sec, ms, name_.c_str()) + 1;
    if (message_size <= 0) {
        return {};
    }
    size_t size = static_cast<size_t>(message_size);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, FORMAT_.c_str(),
                  ts->tm_year + 1900, ts->tm_mon, ts->tm_mday,
                  ts->tm_hour, ts->tm_min, ts->tm_sec, ms, name_.c_str());

    return std::string(buf.get(), buf.get() + size - 1);
}

void Client::runClientForever() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(period_));
        size_t retry_attempt = 0;
        while (retry_attempt <= CONN_RETRY_ && !connectToServer()) {
            ++retry_attempt;
            std::this_thread::sleep_for(std::chrono::seconds(CONN_RETRY_PERIOD_SECONDS_));
        }
        if (retry_attempt > CONN_RETRY_) {
            throw std::runtime_error("Cannot connect to the server");
        }
        std::string message = getMessageInFormat();
        if (message.empty()) {
            continue;
        }
        if (!sendToServer(message)) {
            continue;
        }
        if (!closeConnection()) {
            continue;
        }
    }
}
