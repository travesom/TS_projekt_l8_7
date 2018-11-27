#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"

class ClientUDP : public NodeUDP {
public:
	ClientUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) :NodeUDP(IP, Port1, Port2) {};
	virtual ~ClientUDP() { WSACleanup(); };

	void receive_text_protocol_1(TextProtocol& d) {// odbiera komunikat w ktorym serwer nadaje ID
		char temp_c[62];

		std::cout << "Receiving datagrams...\n";
		const int iResult = recvfrom(RecvSocket, temp_c, 62, 0, (SOCKADDR *)& RecvAddr2, &SenderAddrSize);

		if (iResult == SOCKET_ERROR) {
			wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
		}
		std::string temp(temp_c); temp.resize(62);
		std::cout << "\nRecvBuf is: " << temp << "\n\n";

		std::size_t found = temp.find(HEAD_ID);
		d.ID = stoi(temp.substr(found + 15));
		std::cout << d.ID << std::endl;;
	}
};

