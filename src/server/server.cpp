#include <server.h>
#include <stdexcept>
#include <cstring>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>


Server::Server(const std::string& host, uint16_t port)
              : host_(host)
              , port_(port)
{ }

Server::Server(uint16_t port)
              : port_(port)
{ }

bool Server::startListening() {
    listen_fd_ = socket(DOMAIN_, CONN_TYPE_, 0);
    if (listen_fd_ < 0) {
        return false;
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = DOMAIN_;
    if (host_.empty()) {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(server_addr.sin_family, host_.c_str(), &server_addr.sin_addr.s_addr);
    }
    server_addr.sin_port = htons(port_);

    if (bind(listen_fd_, (sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        return false;
    }

    if (listen(listen_fd_, MAX_CONN_NUM_) < 0) {
        return false;
    }

    return true;
}

void Server::logInfoFromClient(const char* info) const {
    std::lock_guard<std::mutex> lock(log_mtx_);
    std::ofstream log;
    log.open(FILENAME_, std::ios_base::app);
    log << info << "\n";
    log.close();
}

void Server::processConnection(int conn_fd) const {
    char buf[MAX_BUF_SIZE_];
    std::memset(&buf, '\0', MAX_BUF_SIZE_);
    size_t total_received = 0;              // total received bytes
    ssize_t received_bytes = 1;             // number of bytes received in previous recv() call
    while (received_bytes > 0) {
        received_bytes = recv(conn_fd, buf + total_received, sizeof(buf), 0);
        if (received_bytes < 0) {
            break;
        }
        total_received += received_bytes;
    }

    logInfoFromClient(buf);

    close(conn_fd);
}

void Server::runServerForever() {
    if (!startListening()) {
        throw std::runtime_error("Couldn't start listening");
    }

    while (true) {
        int conn_fd = accept(listen_fd_, nullptr, nullptr);
        if (conn_fd < 0) {
            continue;
        }
        std::thread t(&Server::processConnection, this, conn_fd);
        t.detach();
    }
}
