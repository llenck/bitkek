#pragma once

#include <cstdint>

#define bs16 __builtin_bswap16
#define bs32 __builtin_bswap32
#define bs64 __builtin_bswap64

struct ConnectRequest_raw {
	uint64_t prot_id;
	uint32_t action;
	uint32_t trans_id;

	void parse() {
		prot_id = bs64(prot_id);
		action = bs32(action);
		trans_id = bs32(trans_id);
	}
} __attribute__((packed));

struct AnnounceRequest_raw {
	uint64_t conn_id;
	uint32_t action;
	uint32_t trans_id;
	uint8_t info_hash[20];
	uint8_t peer_id[20];
	uint64_t downloaded;
	uint64_t left;
	uint64_t uploaded;
	uint32_t event;
	uint32_t _ip;
	uint32_t key;
	uint32_t num_want;
	uint16_t port;

	void parse() {
		conn_id = bs64(conn_id);
		action = bs32(action);
		trans_id = bs32(trans_id);

		downloaded = bs64(downloaded);
		left = bs64(left);
		uploaded = bs64(uploaded);
		event = bs32(event);
		_ip = bs32(_ip);
		key = bs32(key);
		num_want = bs32(num_want);
		port = bs16(port);
	}
} __attribute__((packed));

struct ScrapeRequest_raw {
	uint64_t conn_id;
	uint32_t action;
	uint32_t trans_id;
	uint8_t info_hash[20];
} __attribute__((packed));

#undef bs16
#undef bs32
#undef bs64
