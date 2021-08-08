#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <stdexcept>
#include <string>
#include <algorithm>
#include <array>
#include <memory>
#include <type_traits>
#include <span>

#include <cstring>
#include <cstdio>
#include <cstdint>

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#include "requests.hpp"

template <int buf_sz, int buf_cnt, bool ipv6 = false>
struct RecvBuffers {
	using sockaddr = std::conditional<ipv6, sockaddr_in6, sockaddr_in>;

	std::array<std::array<uint8_t, buf_sz>, buf_cnt> buffers;
	std::array<struct iovec, buf_cnt> iovecs;
	std::array<struct mmsghdr, buf_cnt> headers;
	std::array<sockaddr, buf_cnt> addrs;

	RecvBuffers() {
		for (int i = 0; i < buf_cnt; i++) {
			iovecs[i].iov_base = buffers[i].data();
			iovecs[i].iov_len = buf_sz;

			headers[i].msg_hdr.msg_name = &addrs[i];
			headers[i].msg_hdr.msg_namelen = sizeof(sockaddr);
			headers[i].msg_hdr.msg_iov = &iovecs[i];
			headers[i].msg_hdr.msg_iovlen = 1;
			headers[i].msg_hdr.msg_control = NULL;
			headers[i].msg_hdr.msg_controllen = 0;
			//headers[i].msg_hdr.msg_flags = 0; // gets written by recvmmsg()
			//headers[i].msg_len = 0; // also written by recvmmsg()
		}
	}
	RecvBuffers(const RecvBuffers& other) = delete;
	RecvBuffers(RecvBuffers&& other) = delete;

	std::span<uint8_t> operator[](off_t off) {
		return {buffers[off].data(), headers[off].msg_len};
	}
};

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
	UdpSocket(UdpSocket&& other) {
		sock = std::exchange(other.sock, -1);
	}
	~UdpSocket() {
		if (sock >= 0) {
			close(sock);
		}
	}

	// Buffer is RecvBuffers<...>
	template<typename Buffer>
	int recvmmsg_into(Buffer& target, bool block = false,
			long timeout_us = 0)
	{
		struct timespec timeout {
			.tv_sec = timeout_us / 1000000,
			.tv_nsec = (timeout_us % 1000000) * 1000,
		};

		return recvmmsg(sock, target.headers.data(), target.headers.size(),
				block? 0 : MSG_WAITFORONE, timeout_us == 0? NULL : &timeout);
	}
};

void handle_req(UdpSocket& s, Request& req) {
	printf("Got request: %s (conn_id: %lu)\n", req.name(), req.conn_id());

	switch (req.type()) {
	case RqType::Connect:
		printf("a\n");
		break;

	case RqType::Announce:
		printf("b\n");
		break;

	default:
		printf("[debug] Unknown request type\n");
		break;
	}
}

void main_loop(UdpSocket& s) {
	auto bufs = std::make_unique<RecvBuffers<1024, 16>>();

	do {
		int msgs = s.recvmmsg_into(*bufs);

		for (int i = 0; i < msgs; i++) {
			std::span<uint8_t> buf = (*bufs)[i];

			std::unique_ptr<Request> req(parse_req(buf));

			if (req) {
				handle_req(s, *req);
			}
			else {
				printf("Got garbage :(\n");
			}
		}
	} while(0);
}

int main() {
	UdpSocket s("127.0.0.1:6969");
	main_loop(s);
}
