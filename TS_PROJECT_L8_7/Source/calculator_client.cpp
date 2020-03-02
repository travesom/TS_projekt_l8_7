#include "calculator_client.hpp"

#include "utils.hpp"

using namespace std;

calculator_client::calculator_client() : udp_node()
{
}

calculator_client::calculator_client(const string& address, const unsigned short& port): udp_node(address, port)
{
	print_messages_ = false;
	print_errors_ = false;
	set_receive_timeout(recv_timeout);
}

calculator_client::~calculator_client() = default;

bool calculator_client::find_server()
{
	string address = "127.0.0.1";

	list<text_protocol> messages;
	messages.emplace_back(text_protocol(utils::get_current_time(), session_id, text_protocol::op_begin));
	text_protocol address_message(utils::get_current_time(), session_id, 0);
	address_message.address = address;
	messages.push_back(address_message);
	send_messages(messages);

	Sleep(100);

	bool find_success = false;
	text_protocol received;
	if (receive_message(received))
	{
		if (received.operation == text_protocol::op_id_session)
		{
			session_id = received.session_id;
			find_success = true;
		}
	}
	else { return false; }

	if (find_success)
	{
		Sleep(100);
		send_message(text_protocol(utils::get_current_time(), session_id, text_protocol::op_ack));
	}

	return true;
}

void calculator_client::end_session()
{
	send_message(text_protocol(utils::get_current_time(), session_id, text_protocol::op_end));
}

list<text_protocol> calculator_client::calculate(const string& arg1,const string& arg2, const text_protocol::operation_enum& operation)
{
	list<text_protocol> messages;
	messages.emplace_back(text_protocol(utils::get_current_time(), session_id, operation));
	text_protocol arg_1_message(utils::get_current_time(), session_id, 0);
	arg_1_message.number = stod(arg1);
	messages.push_back(arg_1_message);
	if(!arg2.empty())
	{
		messages.emplace_back(text_protocol(utils::get_current_time(), session_id, operation));
		text_protocol arg_2_message(utils::get_current_time(), session_id, 0);
		arg_2_message.number = stod(arg2);
		messages.push_back(arg_2_message);
	}
	send_messages(messages);
	
	list<text_protocol> result = receive_messages();
	list<text_protocol> result_filtered;
	for(const text_protocol& message : result)
	{
		if (message.operation != text_protocol::op_id_session
			&& message.status != text_protocol::status_not_found
			&& message.status != text_protocol::status_forbidden
			&& message.status != text_protocol::status_found
			&& message.status != text_protocol::status_history_empty)
		{
			result_filtered.push_back(message);
		}
	}
	return result_filtered;
}

std::list<text_protocol> calculator_client::get_history_by_session_id()
{
	send_message(text_protocol(utils::get_current_time(), session_id, text_protocol::op_history_whole));

	const list<text_protocol> history = receive_messages();
	list<text_protocol> history_filtered;
	for (const text_protocol& message : history)
	{
		if (message.operation != text_protocol::op_id_session
			&& message.status != text_protocol::status_not_found
			&& message.status != text_protocol::status_forbidden
			&& message.status != text_protocol::status_found)
		{
			history_filtered.push_back(message);
		}
	}

	return history_filtered;
}

list<text_protocol> calculator_client::get_history_by_calculation_id(const unsigned& calculation_id)
{
	list<text_protocol> messages;
	messages.emplace_back(text_protocol(utils::get_current_time(), session_id, text_protocol::op_history_id));
	text_protocol id_message(utils::get_current_time(), session_id, 0);
	id_message.calculation_id = calculation_id;
	messages.push_back(id_message);
	send_messages(messages);

	const list<text_protocol> history = receive_messages();
	list<text_protocol> history_filtered;
	for (const text_protocol& message : history)
	{
		if (message.operation != text_protocol::op_id_session
			&& message.status != text_protocol::status_not_found
			&& message.status != text_protocol::status_forbidden
			&& message.status != text_protocol::status_found)
		{
			history_filtered.push_back(message);
		}
	}

	return history_filtered;
}
