#include "LinuxSocket.hpp"

LinuxSocket::LinuxSocket(const uint32_t domain_type, const uint32_t packet_type,
                         const uint32_t proto, const uint16_t port)
    : m_domain_type(domain_type), m_packet_type(packet_type), m_proto(proto),
      m_port(port) {}

LinuxSocket::~LinuxSocket() { socket_close(); }

void LinuxSocket::init_server() {
  m_is_server = true;
  m_server_descriptor = socket(m_domain_type, m_packet_type, m_proto);
  if (m_server_descriptor < 0) {
    std::cerr << "Failed to create socket!: " << strerror(errno) << std::endl;
  }

  uint32_t opt = 1;
  auto sockopt = setsockopt(m_server_descriptor, SOL_SOCKET,
                            SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  if (sockopt < 0) {
    std::cerr << "sockopt failed!: " << strerror(errno) << std::endl;
  }

  if (m_domain_type == LinuxSocket::IPV6) {
    m_address6.sin6_family = m_domain_type;
    m_address6.sin6_addr = in6addr_any;
    m_address6.sin6_port = htons(m_port);
    m_address_length = sizeof(m_address6);
  } else {
    m_address.sin_family = m_domain_type;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(m_port);
    m_address_length = sizeof(m_address);
  }
}

void LinuxSocket::socket_write(const void *data, size_t size) {
  auto write_status = send(m_client_socket, data, size, 0);
  if (write_status < 0) {
    std::cerr << "Failed wrinting to socket!: " << strerror(errno) << std::endl;
  }
}

void LinuxSocket::socket_sendto(const void *data, size_t buffer_len,
                                const uint16_t port) {
  m_address.sin_port = htons(port);
  auto send_status =
      sendto(m_server_descriptor, data, buffer_len, 0,
             (const struct sockaddr *)&m_address, m_address_length);
  if (send_status < 0) {
    std::cerr << "Failed sendto!: " << strerror(errno) << std::endl;
  }
  m_address.sin_port = htons(m_port);
}

void LinuxSocket::socket_read(void *buffer, const size_t buffer_len) {
  auto read_status = read(m_client_socket, buffer, buffer_len);
  if (read_status < 0) {
    std::cerr << "Failed reading socket!: " << strerror(errno) << std::endl;
  }
}

void LinuxSocket::socket_recvfrom(void *buffer, const size_t buffer_len,
                                  ssize_t &recv_len) {
  socklen_t len;
  recv_len = recvfrom(m_client_socket, buffer, buffer_len, 0,
                      (struct sockaddr *)&m_address, &len);
  if (recv_len < 0) {
    std::cerr << "Failed recvfrom!: " << strerror(errno) << std::endl;
  }
}

void LinuxSocket::socket_bind() {
  auto binding = bind((m_is_server) ? m_server_descriptor : m_client_socket,
                      get_address(), m_address_length);
  if (binding < 0) {
    std::cerr << "Failed to bind socket!: " << strerror(errno) << std::endl;
  }
}

void LinuxSocket::socket_connect(const std::string &dns_address) {
  if ((m_client_socket = socket(m_domain_type, m_packet_type, m_proto)) < 0) {
    std::cout << "Failed creating socket!: " << strerror(errno) << std::endl;
  }

  if (m_domain_type == LinuxSocket::IPV6) {
    m_address6.sin6_family = m_domain_type;
    inet_pton(AF_INET6, dns_address.c_str(), &m_address6.sin6_addr);
    m_address6.sin6_port = htons(m_port);
    m_address_length = sizeof(m_address6);
  } else {
    m_address.sin_family = m_domain_type;
    m_address.sin_port = htons(m_port);
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address_length = sizeof(m_address);
  }

  if (m_packet_type == LinuxSocket::UDP) {
    socket_bind();
  } else {
    if (inet_pton(m_domain_type, dns_address.c_str(), &m_address.sin_addr) <=
        0) {
      std::cerr << "Invalid address!: " << strerror(errno) << std::endl;
    }

    auto connection_status =
        connect(m_client_socket, get_address(), m_address_length);
    if (connection_status < 0) {
      std::cerr << "Failed connecting to socket!: " << strerror(errno)
                << std::endl;
    }
  }
}

void LinuxSocket::socket_listen(const int max_connections) {
  auto listening = listen(m_server_descriptor, max_connections);
  if (listening < 0) {
    std::cerr << "Failed listening to socket!: " << strerror(errno)
              << std::endl;
  }
}

void LinuxSocket::socket_accept() {
  auto client_status =
      (m_client_socket =
           accept(m_server_descriptor, get_address(), &m_address_length));
  if (client_status < 0) {
    std::cout << "Failed accepting incoming connection!: " << strerror(errno)
              << std::endl;
  }
}

void LinuxSocket::socket_close() {
  if (m_is_server) {
    close(m_server_descriptor);
  }
  close(m_client_socket);
}

struct sockaddr *LinuxSocket::get_address() {
  if (m_domain_type == LinuxSocket::IPV4) {
    return (struct sockaddr *)&m_address;
  }
  return (struct sockaddr *)&m_address6;
}
