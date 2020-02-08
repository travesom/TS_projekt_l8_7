#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "udp_node.hpp"

#include "sync_cout.hpp"

using namespace std;

udp_node::udp_node(const std::string& address, const unsigned short& port)
{
	const int i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data_);
	if (i_result != NO_ERROR)
	{
		if (print_messages_) { s_cout << "WSAStartup failed with error " << i_result << "\n"; }
		return;
	}

	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket_ == INVALID_SOCKET)
	{
		if (print_messages_) { s_cout << "Socket creation failed with error: " << WSAGetLastError() << "\n"; }
		WSACleanup();
		return;
	}

	other_addr_.sin_family = AF_INET;
	other_addr_.sin_port = htons(port);
	other_addr_.sin_addr.s_addr = inet_addr(address.c_str());
}

udp_node::~udp_node() { WSACleanup(); }

void udp_node::set_receive_timeout(const int& timeout) const
{
	/*
	 * Source: https:// stackoverflow.com/questions/1824465/set-timeout-for-winsock-recvfrom
	 */
	setsockopt(
		socket_,
		SOL_SOCKET,
		SO_RCVTIMEO,
		reinterpret_cast<const char*>(&timeout),
		sizeof(timeout)
	);
}

bool udp_node::receive_message(string& received)
{
	received.clear();
	char buffer[1024];
	int send_addr_length = sizeof(other_addr_);

	fill(begin(buffer), end(buffer), '\0');
	const int i_result = recvfrom(socket_, buffer, sizeof(buffer), 0,
	                              reinterpret_cast<SOCKADDR*>(&other_addr_),
	                              &send_addr_length);
	const int last_error = WSAGetLastError();
	if (i_result == SOCKET_ERROR)
	{
		if (print_errors_) { s_cout << "Receiving failed with error: " << last_error << "\n"; }
		return false;
	}

	string result;
	for (const char& i : buffer)
	{
		if (i != '\0') { result.push_back(i); }
		else
		{
			result.push_back(i);
			break;
		}
	}

	if (result.empty()) { return false; }

	received = result;
	if (print_messages_) { s_cout << "Received: " << result << endl; }
	return true;
}

bool udp_node::receive_message(text_protocol& received)
{
	string received_string;
	if (!receive_message(received_string)) { return false; }
	received = text_protocol(received_string);
	return true;
}

bool udp_node::send_message(const text_protocol& message, const text_protocol::field_enum& field)
{
	const string send_str = message.serialize(field);
	if (print_messages_) { s_cout << "Sending: " << send_str << endl; }

	const int i_result = sendto(socket_, send_str.c_str(), int(send_str.length()), 0,
	                            reinterpret_cast<SOCKADDR*>(&other_addr_), sizeof(other_addr_));
	if (i_result == SOCKET_ERROR)
	{
		if (print_errors_) { s_cout << "Sending failed with error: " << WSAGetLastError() << "\n"; }
		return false;
	}
	return true;
}

bool udp_node::send_message(const text_protocol& message)
{
	return send_message(message, message.get_field());
}

void udp_node::send_messages(const std::list<text_protocol>& messages)
{
	unsigned sequence_number = unsigned(messages.size() - 1);
	for (text_protocol message : messages)
	{
		message.sequence_number = sequence_number;
		send_message(message);
		sequence_number--;
	}
}

list<text_protocol> udp_node::receive_messages()
{
	list<text_protocol> result;
	auto sequence_number = unsigned(-1);
	unsigned fail_count = 0;
	while (sequence_number != 0)
	{
		text_protocol received;
		if (receive_message(received))
		{
			if (sequence_number != unsigned(-1) && sequence_number - 1 != received.sequence_number)
			{
				return list<text_protocol>();
			}
			sequence_number = received.sequence_number;
			result.push_back(received);
		}
		else
		{
			fail_count++;
			if (fail_count >= 4) { return list<text_protocol>(); }
		}
	}
	if (print_messages_) { s_cout << "Finished receiving messages" << endl; }
	return result;
}
