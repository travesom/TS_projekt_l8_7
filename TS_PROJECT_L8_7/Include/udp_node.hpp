#pragma once
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include "text_protocol.hpp"

#include <list>
#include <winsock2.h>

class udp_node
{
protected:
	static const unsigned buf_len = 1024;
	static const unsigned recv_timeout = 2500;

	bool print_messages_ = true;
	bool print_errors_ = true;
	WSADATA wsa_data_{};
	SOCKET socket_{};
	sockaddr_in other_addr_{};

	udp_node() = default;

	explicit udp_node(const std::string& address, const unsigned short& port);
	~udp_node();

	void set_receive_timeout(const int& timeout) const;

	bool receive_message(std::string& received);

	bool receive_message(text_protocol& received);

	bool send_message(const text_protocol& message, const text_protocol::field_enum& field);

	bool send_message(const text_protocol& message);

	void send_messages(const std::list<text_protocol>& messages);

	std::list<text_protocol> receive_messages();
};
