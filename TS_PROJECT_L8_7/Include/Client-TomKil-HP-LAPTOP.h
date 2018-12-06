#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include "Protocol.h"

#pragma comment(lib, "Ws2_32.lib")// Link with ws2_32.lib
class ClientUDP{
private:
public: //Tymczasowo
	WSADATA wsaData{};
	SOCKET SendSocket;
	SOCKET RecvSocket;
	sockaddr_in RecvAddr{};
	sockaddr_in RecvAddr2{};

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	char SendBuf[1024];
	char RecvBuf[1024];
	int BufLen = 1024;

public:
	ServerUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) {
		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			std::cout << "WSAStartup failed with error " << iResult << "\n";
			return;
		}
		//-----------------------------------------------
		// Create a receiver socket to receive datagrams
		RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (RecvSocket == INVALID_SOCKET) {
			std::cout << "socket failed with error " << WSAGetLastError << "\n";
			return;
		}
		//-----------------------------------------------
		// Bind the socket to any address and the specified port.
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(Port1);
		RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		//---------------------------------------------
		// Create a socket for sending data
		SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (SendSocket == INVALID_SOCKET) {
			std::cout << "socket failed with error: " << WSAGetLastError() << "\n";
			WSACleanup();
			return;
		}
		//---------------------------------------------
		// Set up the RecvAddr structure with the IP address
		// and the specified port number.
		RecvAddr2.sin_family = AF_INET;
		RecvAddr2.sin_port = htons(Port2);
		RecvAddr2.sin_addr.s_addr = inet_addr(IP.c_str());

		iResult = bind(RecvSocket, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
		if (iResult != 0) {
			std::cout << "bind failed with error " << WSAGetLastError() << "\n";
			return;
		}
	};
	virtual ~ServerUDP() { WSACleanup(); };

	void send_text_protocol(const std::string& data) {
		strcpy_s(SendBuf, data.c_str());
		const int iResult = sendto(SendSocket, SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr2, sizeof(RecvAddr2));
		if (iResult == SOCKET_ERROR) {
			std::cout << "sendto failed with error: " << WSAGetLastError() << "\n";
			closesocket(SendSocket);
			WSACleanup();
			return;
		}
		std::cout << SendBuf << std::endl;
	}

	void receive_text_protocol(TextProtocol&) {
		std::cout << "Receiving datagrams...\n";
		const int iResult = recvfrom(RecvSocket, RecvBuf, BufLen, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			std::cout << "recvfrom failed with error " << WSAGetLastError() << "\n";
			return;
		}
		std::cout << "\nRecvBuf is: " << RecvBuf << "\n\n";
	}
};