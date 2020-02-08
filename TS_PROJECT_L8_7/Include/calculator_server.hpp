#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "udp_node.hpp"

#include <list>
#include <map>
#include <set>

class udp_server : public udp_node
{
private:
	unsigned calculation_id_ = 1;

	static std::map<text_protocol::operation_enum, bool (*)(const double&, const double&, double&)> operations_map_;
	std::map<unsigned int, std::pair<unsigned int, std::list<text_protocol>>> history_;
	std::set<unsigned int> session_ids_;
	const unsigned short port_;
	sockaddr_in server_addr_{};


public:
	explicit udp_server(const unsigned short& port);

	bool start_session();

	bool handle_session();

private:
	bool bind_to_address(const std::string& address);

	bool wait_for_op_begin(unsigned& session_id);

	bool wait_for_op_ack(const unsigned& session_id);

	unsigned calculate_session_id(const unsigned& client_session_id);
	
	bool listen_for_client();

	static bool add(const double& argument1, const double& argument2, double& result);

	static bool subtract(const double& argument1, const double& argument2, double& result);

	static bool multiply(const double& argument1, const double& argument2, double& result);

	static bool divide(const double& argument1, const double& argument2, double& result);

	static bool factorial(const double& argument1, const double& ignored, double& result);

	void calculation(bool (*calc_function)(const double&, const double&, double&), const unsigned& session_id);

	std::list<text_protocol> get_history_by_session_id(const unsigned& session_id);

	void history_by_session_id(const unsigned& session_id);

	void history_by_calc_id(const unsigned& session_id, const unsigned& calc_id);

};
