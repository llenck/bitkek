#pragma once

#include <span>

#include "req_types.hpp"

enum RqType {
	Connect, Announce, Scrape
};

struct Request {
	virtual const char* name() = 0;
	virtual RqType type() = 0;
	virtual uint64_t conn_id() = 0;

	virtual void parse() = 0;
};

struct ConnectRequest: Request {
	ConnectRequest_raw* inner;

	ConnectRequest(std::span<uint8_t> buf);
	ConnectRequest(const ConnectRequest& other) = delete;

	const char* name();
	RqType type();
	uint64_t conn_id();
	void parse();
};

struct AnnounceRequest: Request {
	AnnounceRequest_raw* inner;

	AnnounceRequest(std::span<uint8_t> buf);
	AnnounceRequest(const ConnectRequest& other) = delete;

	const char* name();
	RqType type();
	uint64_t conn_id();
	void parse();
};

Request* parse_req(std::span<uint8_t> buf);
