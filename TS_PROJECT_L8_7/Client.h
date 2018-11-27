#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"

class ClientUDP : public NodeUDP {
	unsigned int sessionId = 0;

public:
	ClientUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) :NodeUDP(IP, Port1, Port2) {};
	virtual ~ClientUDP() { WSACleanup(); };

	bool start_session() {
		TextProtocol d('p', 0, 0, GET_CURRENT_TIME());

		//¯¹danie o rozpoczêcie sesji
		if (!send_text_protocol(d, id_message_size)) {
			std::cout << "B³¹d wysy³ania.\n";
			return false;
		}

		//Odbieranie id
		std::string received;
		receive_text_protocol(received, id_message_size);
		d.from_string(received);
		sessionId = d.ID;

		return true;
	}
};

