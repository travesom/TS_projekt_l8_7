#include "calculator_server.hpp"

#include "sync_cout.hpp"
#include "utils.hpp"

#include <array>

using namespace std;

map<text_protocol::operation_enum, bool (*)(const double&, const double&, double&)> udp_server::operations_map_{
	{text_protocol::op_add, &add},
	{text_protocol::op_subt, &subtract},
	{text_protocol::op_multp, &multiply},
	{text_protocol::op_div, &divide},
	{text_protocol::op_fact, &factorial}
};

udp_server::udp_server(const unsigned short& port): udp_node("0.0.0.0", port), port_(htons(port))
{
	print_messages_ = true;
	print_errors_ = false;
	server_addr_.sin_family = AF_INET;
	set_receive_timeout(recv_timeout);
}

bool udp_server::start_session()
{
	bind_to_address("0.0.0.0");
	return listen_for_client();
}

bool udp_server::bind_to_address(const string& address)
{
	server_addr_.sin_addr.s_addr = inet_addr(address.c_str());
	server_addr_.sin_port = port_;

	closesocket(socket_);
	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	s_cout << "Binding to address: " << inet_ntoa(server_addr_.sin_addr) << " : " << server_addr_.sin_port << '\n';
	Sleep(100);
	const int i_result = bind(socket_, reinterpret_cast<SOCKADDR*>(&server_addr_), sizeof(server_addr_));
	const int last_error = WSAGetLastError();
	set_receive_timeout(1000);

	if (i_result != 0)
	{
		s_cout << "Binding failed with error: " << last_error << "\n";
		return false;
	}

	s_cout << "Binding successful\n";
	return true;
}

bool udp_server::wait_for_op_begin(unsigned& session_id)
{
	string received;
	int8_t fail_count = 0;
	while (true)
	{
		const list<text_protocol> messages = receive_messages();

		if (messages.empty())
		{
			fail_count++;
			s_cout << "Attempts left: " << 11 - fail_count << '\n';
			if (fail_count >= 11) { break; }
			continue;
		}
		if (messages.size() < 2) { continue; }
		if (messages.begin()->operation == text_protocol::op_begin
			&& next(messages.begin(), 1)->get_field() == text_protocol::field_address)
		{
			if (!bind_to_address(next(messages.begin(), 1)->address))
			{
				s_cout << "Session creation unsuccessful.\n\n";
				return false;
			}
			s_cout << "Session creation successful.\n\n";
			session_id = messages.begin()->session_id;
			return true;
		}
	}
	return false;
}

bool udp_server::wait_for_op_ack(const unsigned& session_id)
{
	int8_t fail_count = 0;
	while (true)
	{
		Sleep(200);

		send_message(text_protocol(utils::get_current_time(), session_id, text_protocol::op_id_session));

		s_cout << "Waiting for ACK...\n";
		text_protocol message;
		if (!receive_message(message))
		{
			fail_count++;
			if (fail_count >= 11) { return false; }
		}
		else if (message.get_field() == text_protocol::field_operation && message.operation ==
			text_protocol::op_ack)
		{
			session_ids_.insert(session_id);
			s_cout << "Used session ids:\n";
			for (const unsigned int& id : session_ids_) { s_cout << " - " << id << '\n'; }
			s_cout << "Session creation successful.\n\n";
			return true;
		}
	}
}

unsigned udp_server::calculate_session_id(const unsigned& client_session_id)
{
	unsigned session_id = client_session_id;

	if (session_ids_.find(session_id) == session_ids_.end() || session_id == 0)
	{
		session_id = utils::rand_int(1, 999);
		while (true)
		{
			if (session_ids_.find(session_id) == session_ids_.end()) { break; }
			session_id++;
			if (session_id > 999) { session_id = utils::rand_int(1, 999); }
		}
	}
	return session_id;
}

bool udp_server::listen_for_client()
{
	s_cout << "\nListening for clients.\n";
	unsigned client_session_id;
	if (!wait_for_op_begin(client_session_id))
	{
		s_cout << "Session creation unsuccessful.\n\n";
		return false;
	}

	const unsigned session_id = calculate_session_id(client_session_id);
	return wait_for_op_ack(session_id);
}

bool udp_server::add(const double& argument1, const double& argument2, double& result)
{
	const double temp_result = argument1 + argument2;
	if (temp_result > 2147483647.0 || temp_result < -2147483647.0) { return false; }
	result = temp_result;
	return true;
}

bool udp_server::subtract(const double& argument1, const double& argument2, double& result)
{
	const double temp_result = argument1 - argument2;
	if (temp_result > 2147483647.0 || temp_result < -2147483647.0) { return false; }
	result = double(temp_result);
	return true;
}

bool udp_server::multiply(const double& argument1, const double& argument2, double& result)
{
	double temp_result = 0;
	for (int i = abs(int(argument2)); i > 0; i--)
	{
		temp_result += argument1;
		if (temp_result >= 2147483647.0 || temp_result <= -2147483647.0) { return false; }
	}
	result = argument1 * argument2;
	return true;
}

bool udp_server::divide(const double& argument1, const double& argument2, double& result)
{
	result = double(argument1) / double(argument2);
	result = round(result * 10000) / 10000;
	return true;
}

bool udp_server::factorial(const double& argument1, const double& ignored, double& result)
{
	if (argument1 == 0)
	{
		result = 1;
	}
	else
	{
		double temp = 1;
		for (unsigned i = 1; i <= argument1; i++)
		{
			temp = temp * i;
			if (temp > 4294967295.0) { return false; }
		}
		result = unsigned(temp);
	}
	return true;
}

void udp_server::calculation(bool (* calc_function)(const double&, const double&, double&), const unsigned& session_id)
{
	const list<text_protocol> received_messages = receive_messages();

	array<double, 2> args{0.0, 0.0};
	unsigned arg_num = 0;
	for (const text_protocol& message : received_messages)
	{
		if (message.get_field() == text_protocol::field_number)
		{
			history_[calculation_id_].second.push_back(message);
			args[arg_num] = message.number;
			arg_num++;
		}
	}

	double result;

	list<text_protocol> result_messages;

	result_messages.emplace_back(text_protocol(utils::get_current_time(), session_id, text_protocol::op_status));
	history_[calculation_id_].second.push_back(result_messages.back());

	if (!calc_function(args[0], args[1], result))
	{
		result_messages.emplace_back(text_protocol(utils::get_current_time(), session_id,
		                                           text_protocol::status_out_of_range));
		history_[calculation_id_].second.push_back(result_messages.back());
	}
	else
	{
		result_messages.emplace_back(
			text_protocol(utils::get_current_time(), session_id, text_protocol::status_success));
		history_[calculation_id_].second.push_back(result_messages.back());

		text_protocol result_protocol(utils::get_current_time(), session_id, 0);
		result_protocol.number = result;
		result_messages.push_back(result_protocol);
		history_[calculation_id_].second.push_back(result_messages.back());
	}

	text_protocol calc_id_protocol(utils::get_current_time(), session_id, 0);
	calc_id_protocol.calculation_id = calculation_id_;
	result_messages.push_back(calc_id_protocol);
	history_[calculation_id_].second.push_back(result_messages.back());

	send_messages(result_messages);
}

list<text_protocol> udp_server::get_history_by_session_id(const unsigned& session_id)
{
	list<text_protocol> result;
	for (const auto& elem : history_)
	{
		if (elem.second.first != session_id) { continue; }
		for (const auto& protocol : elem.second.second) { result.push_back(protocol); }
	}
	return result;
}

void udp_server::history_by_session_id(const unsigned& session_id)
{
	list<text_protocol> session_history = get_history_by_session_id(session_id);

	if (session_history.empty())
	{
		send_message(text_protocol(utils::get_current_time(), session_id, text_protocol::status_history_empty));
		return;
	}

	const text_protocol status_message(utils::get_current_time(), session_id, text_protocol::status_found);
	session_history.insert(session_history.begin(), status_message);

	send_messages(session_history);
}

void udp_server::history_by_calc_id(const unsigned& session_id, const unsigned& calc_id)
{
	if (history_.find(calc_id) == history_.end())
	{
		send_message(text_protocol(utils::get_current_time(), session_id, text_protocol::status_not_found));
		return;
	}

	list<text_protocol> messages;
	if (history_[calc_id].first != session_id)
	{
		messages.emplace_back(text_protocol(utils::get_current_time(), session_id, text_protocol::op_status));
		messages.emplace_back(text_protocol(utils::get_current_time(), session_id, text_protocol::status_forbidden));
		send_messages(messages);
	}
	else if (history_[calc_id].first == session_id)
	{
		messages.emplace_back(text_protocol(utils::get_current_time(), session_id, text_protocol::op_status));
		messages.emplace_back(text_protocol(utils::get_current_time(), session_id, text_protocol::status_found));

		for (const text_protocol& message : history_[calc_id].second)
		{
			messages.push_back(message);
		}
		send_messages(messages);
	}
}

bool udp_server::handle_session()
{
	string received;
	while (true)
	{
		if (!receive_message(received)) { continue; }

		text_protocol operation_message(received);

		if (operation_message.get_field() != text_protocol::field_operation) { continue; }

		switch (operation_message.operation)
		{
		case text_protocol::op_begin:
			return false;
		case text_protocol::op_end:
			return true;
		case text_protocol::op_history_whole:
			history_by_session_id(operation_message.session_id);
			break;
		case text_protocol::op_history_id:
			receive_message(received);
			history_by_calc_id(operation_message.session_id, text_protocol(received).calculation_id);
			break;
		case text_protocol::op_add:
		case text_protocol::op_subt:
		case text_protocol::op_div:
		case text_protocol::op_multp:
		case text_protocol::op_fact:
			history_[calculation_id_].first = operation_message.session_id;
			history_[calculation_id_].second.push_back(operation_message);
			calculation(operations_map_[operation_message.operation], operation_message.session_id);
			calculation_id_++;
			break;
		default: ;
		}
	}
}
