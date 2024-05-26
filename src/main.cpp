#include <functional>
#include <future>
#include <iostream>
#include <thread>

#include "LinuxSocket.hpp"

void server() {
  LinuxSocket server_socket(LinuxSocket::IPV4, LinuxSocket::TCP, 0, 6779);
  std::string msg = "Hello Socket Server!\n";
  server_socket.init_server();
  server_socket.socket_bind();
  server_socket.socket_listen(3);
  server_socket.socket_accept();
  server_socket.socket_write(msg.c_str(), msg.size());
}

void client() {
  std::string address = "127.0.0.1";
  char buffer[1024];

  LinuxSocket client_socket(LinuxSocket::IPV4, LinuxSocket::TCP, 0, 6779);
  client_socket.socket_connect(address);

  client_socket.socket_read(buffer, sizeof(buffer) / sizeof(char) - 1);

  std::cout << "Client read: " << buffer << std::endl;
}

int main(int argc, const char *argv[]) {
  std::future<void> res = std::async(std::launch::async, server);
  std::this_thread::sleep_for(std::chrono::seconds(3));

  client();

  res.wait();

  return 0;
}