#ifndef LINUXSOCKET_HPP
#define LINUXSOCKET_HPP

#include "Socket.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

class LinuxSocket : public Socket {
public:
  using SocketAddress = struct sockaddr_in;

  const static uint32_t UDP = SOCK_DGRAM;
  const static uint32_t TCP = SOCK_STREAM;
  const static uint32_t IPV4 = AF_INET;
  const static uint32_t IPV6 = AF_INET6;

public:
  LinuxSocket(const uint32_t domain_type, const uint32_t packet_type,
              const uint32_t proto, const uint16_t port);
  ~LinuxSocket();

  void init_server() override;
  void socket_write(const void *data, size_t size) override;
  void socket_sendto(const void *data, size_t buffer_len,
                     const uint16_t port) override;
  void socket_read(void *buffer, const size_t buffer_len) override;
  void socket_recvfrom(void *buffer, const size_t buffer_len,
                       ssize_t &recv_len) override;
  void socket_bind() override;
  void socket_connect(const std::string &dns_address) override;
  void socket_listen(const int max_connections) override;
  void socket_accept() override;
  void socket_close() override;

private:
  bool m_is_server{false};
  uint32_t m_socket_descriptor{0};
  uint32_t m_server_descriptor{0};
  uint32_t m_client_socket{0};
  uint32_t m_domain_type{0};
  uint32_t m_packet_type{0};
  uint32_t m_proto{0};
  uint16_t m_port{0};
  SocketAddress m_address;
  socklen_t m_address_length;
};

#endif