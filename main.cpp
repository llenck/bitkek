#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <stdexcept>
#include <string>
#include <algorithm>
#include <cstring>
#include <cstdio>

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

struct UdpSocket {
	int sock = -1;

	UdpSocket(std::string addr) {
		size_t colon_idx = addr.find(':');
		int port;
		if (colon_idx == std::string::npos) {
			port = 6969;
		}
		else {
			port = atoi(addr.c_str() + colon_idx + 1);
			addr[colon_idx] = '\0';
		}

		struct sockaddr_in bind_addr;
		bind_addr.sin_family = AF_INET;
		bind_addr.sin_port = htons(port);
		memset(&bind_addr.sin_zero, 0, 8);

		if (!inet_aton(addr.c_str(), &bind_addr.sin_addr))
			throw std::runtime_error("Couldn't parse address string");

		sock = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
		if (sock < 0)
			throw new std::runtime_error("Couldn't create socket");

		if (bind(sock, (const struct sockaddr*)&bind_addr, sizeof(bind_addr)) == -1)
			throw new std::runtime_error("Couldn't bind socket");

		int a = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a)) == -1)
			throw new std::runtime_error("Couldn't set SO_REUSEADDR");
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &a, sizeof(a)) == -1)
			throw new std::runtime_error("Couldn't set SO_REUSEPORT");
	}
	UdpSocket(const UdpSocket& other) = delete;
	UdpSocket(UdpSocket& other) {
		sock = std::exchange(other.sock, -1);
	}
	~UdpSocket() {
		if (sock >= 0) {
			close(sock);
		}
	}
};

int main() {
	UdpSocket s("127.0.0.1:1234");
	getchar();
}
