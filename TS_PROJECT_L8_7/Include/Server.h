#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"
#include <random>
#include <ctime>

inline int randInt(const int &min, const int &max) {
	if (max <= min) return min;
	std::random_device rd;
	std::mt19937 gen(rd());
	const std::uniform_int_distribution<int > d(min, max);
	return d(gen);
}

class ServerUDP : public NodeUDP {
public:
	ServerUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) : NodeUDP(IP, Port1, Port2) {};
	virtual ~ServerUDP() { WSACleanup(); };

	bool start_session() {
		//Czekanie na ¿¹danie sesji
		std::string received;
		receive_text_protocol(received);

		//Wys³anie ID
		const TextProtocol d('p', 0, randInt(10, 99), GET_CURRENT_TIME());

		if (!send_text_protocol(d, 0)) {
			std::cout << "B³¹d wysy³ania.\n";
			return false;
		}
		return true;
	}
};
