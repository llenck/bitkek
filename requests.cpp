#include "requests.hpp"

#include <stdexcept>

Request* parse_req(std::span<uint8_t> buf) {
	if (buf.size() < 16) {
		printf("[debug] message too short: only %zd bytes\n", buf.size());
		return NULL;
	}

	uint32_t action = __builtin_bswap32(((uint32_t*)buf.data())[2]);

	Request* ret;

	switch (action) {
	case 0:
		ret = new ConnectRequest(buf);
		break;
	case 1:
		ret = new AnnounceRequest(buf);
		break;
//	case 2:
//		ret = new ScrapeRequest;
//		break;

	default:
		printf("[debug] unknown action: %u\n", action);
		return NULL;
		break;
	}

	ret->parse();

	return ret;
}

ConnectRequest::ConnectRequest(std::span<uint8_t> buf) {
	if (buf.size() < sizeof(*inner))
		throw std::runtime_error("Request to small for given type");

	inner = (ConnectRequest_raw*)buf.data();
}

const char* ConnectRequest::name() {
	return "Connect";
}

RqType ConnectRequest::type() {
	return RqType::Connect;
}

uint64_t ConnectRequest::conn_id() {
	return inner->prot_id;
}

void ConnectRequest::parse() {
	inner->parse();
}

AnnounceRequest::AnnounceRequest(std::span<uint8_t> buf) {
	if (buf.size() < sizeof(*inner))
		throw std::runtime_error("Request to small for given type");

	inner = (AnnounceRequest_raw*)buf.data();
}

const char* AnnounceRequest::name() {
	return "Announce";
}

RqType AnnounceRequest::type() {
	return RqType::Announce;
}

uint64_t AnnounceRequest::conn_id() {
	return inner->conn_id;
}

void AnnounceRequest::parse() {
	inner->parse();
}
