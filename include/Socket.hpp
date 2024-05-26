#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstddef>
#include <inttypes.h>
#include <iostream>

class Socket {
public:
  Socket() = default;
  virtual ~Socket() {}

  virtual void init_server() {}
  virtual void socket_write(const void *data, size_t size) {}
  virtual void socket_sendto(const void *data, size_t buffer_len,
                             const uint16_t port) {};
  virtual void socket_read(void *buffer, const size_t buffer_len) {}
  virtual void socket_recvfrom(void *buffer, const size_t buffer_len,
                               ssize_t &recv_len) {}
  virtual void socket_bind() {}
  virtual void socket_connect(const std::string &dns_address) {}
  virtual void socket_listen(const int max_connections) {}
  virtual void socket_accept() {}
  virtual void socket_close() {}

private:
};

#endif