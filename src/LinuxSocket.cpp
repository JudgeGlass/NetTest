#include "LinuxSocket.hpp"

LinuxSocket::LinuxSocket(const uint32_t domain_type, const uint32_t packet_type,
                         const uint32_t proto, const uint16_t port)
    : m_domain_type(domain_type), m_packet_type(packet_type), m_proto(proto),
      m_port(port) {}

LinuxSocket::~LinuxSocket() {}

void LinuxSocket::init_server() {
  m_is_server = true;
  m_server_descriptor = socket(PF_INET, SOCK_STREAM, 0);
  if (m_server_descriptor < 0) {
    std::cerr << "Failed to create socket!: " << strerror(errno) << std::endl;
  }

  uint32_t opt = 1;
  auto sockopt = setsockopt(m_server_descriptor, SOL_SOCKET,
                            SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  if (sockopt < 0) {
    std::cerr << "sockopt failed! ERRNO: " << strerror(errno) << std::endl;
  }

  m_address.sin_family = m_domain_type;
  m_address.sin_addr.s_addr = INADDR_ANY;
  m_address.sin_port = htons(m_port);
  m_address_length = sizeof(m_address);
}

void LinuxSocket::socket_write(const void *data, size_t size) {
  auto write_status = send(m_client_socket, data, size, 0);
  if (write_status < 0) {
    std::cerr << "Failed wrinting to socket!" << std::endl;
  }
}

void LinuxSocket::socket_read(void *buffer, const size_t buffer_len) {
  auto read_status = read(m_client_socket, buffer, buffer_len);
  if (read_status < 0) {
    std::cerr << "Failed reading socket!" << std::endl;
  }
}

void LinuxSocket::socket_bind() {
  auto binding = bind(m_server_descriptor, (struct sockaddr *)&m_address,
                      sizeof(m_address));
  if (binding < 0) {
    std::cerr << "Failed to bind socket!" << std::endl;
  }
}

void LinuxSocket::socket_connect(const std::string &dns_address) {
  if ((m_client_socket = socket(m_domain_type, m_packet_type, m_proto)) < 0) {
    std::cout << "Failed creating socket!" << std::endl;
  }

  m_address.sin_family = m_domain_type;
  m_address.sin_port = htons(m_port);

  if (inet_pton(m_domain_type, dns_address.c_str(), &m_address.sin_addr) <= 0) {
    std::cerr << "Invalid address!" << std::endl;
  }

  auto connection_status = connect(
      m_client_socket, (struct sockaddr *)&m_address, sizeof(m_address));
  if (connection_status < 0) {
    std::cerr << "Failed connecting to socket!" << std::endl;
  }
}

void LinuxSocket::socket_listen(const int max_connections) {
  auto listening = listen(m_server_descriptor, max_connections);
  if (listening < 0) {
    std::cerr << "Failed listening to socket!" << std::endl;
  }
}

void LinuxSocket::socket_accept() {
  auto client_status = (m_client_socket = accept(m_server_descriptor,
                                                 (struct sockaddr *)&m_address,
                                                 &m_address_length));
  if (client_status < 0) {
    std::cout << "Failed accepting incoming connection!" << std::endl;
  }
}

void LinuxSocket::socket_close() {
  if (m_is_server) {
    close(m_server_descriptor);
  }
  close(m_client_socket);
}
