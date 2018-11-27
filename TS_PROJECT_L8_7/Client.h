#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"

class ClientUDP : public NodeUDP {
public:
	ClientUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) {
		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			std::cout << "WSAStartup niepowiod³o siê z b³êdem: " << iResult << "\n";
			return;
		}
		//-----------------------------------------------
		// Create a receiver socket to receive datagrams
		SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (SendSocket == INVALID_SOCKET) {
			std::cout << "socket niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			WSACleanup();
			return;
		}
		//-----------------------------------------------
		// Bind the socket to any address and the specified port.
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(Port1);
		RecvAddr.sin_addr.s_addr = inet_addr(IP.c_str());

		//---------------------------------------------
		// Create a socket for sending data
		RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (RecvSocket == INVALID_SOCKET) {
			std::cout << "socket failed with error " << WSAGetLastError() << "\n";
			return;
		}
		//---------------------------------------------
		// Set up the RecvAddr structure with the IP address
		// and the specified port number.
		RecvAddr2.sin_family = AF_INET;
		RecvAddr2.sin_port = htons(Port2);
		RecvAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
	};
	virtual ~ClientUDP() { WSACleanup(); };

	bool send_text_protocol(const std::string& data) {
		strcpy_s(SendBuf, data.c_str());
		const int iResult = sendto(SendSocket, SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy³anie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			closesocket(SendSocket);
			WSACleanup();
			return false;
		}
		std::cout << SendBuf << std::endl;
		return true;
	}

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

