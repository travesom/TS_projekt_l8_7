#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "udp_node.hpp"

#include <array>

class calculator_client : public udp_node
{
public:
	unsigned session_id = 0;

	calculator_client();

	explicit calculator_client(const std::string& address, const unsigned short& port);

	~calculator_client();

	bool find_server();

	void end_session();

	std::list<text_protocol> calculate(const std::string& arg1, const std::string& arg2,
	                                   const text_protocol::operation_enum& operation);

	std::list<text_protocol> get_history_by_session_id();

	std::list<text_protocol> get_history_by_calculation_id(const unsigned& calculation_id);
};
