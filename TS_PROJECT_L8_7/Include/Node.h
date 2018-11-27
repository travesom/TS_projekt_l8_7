#pragma once
#include <winsock2.h>
#include <iostream>
#include "Protocol.h"

#pragma comment(lib, "Ws2_32.lib")// Link with ws2_32.lib
class NodeUDP{
protected:
	WSADATA wsaData{};
	SOCKET SendSocket;
	SOCKET RecvSocket;
	sockaddr_in RecvAddr1{};
	sockaddr_in RecvAddr2{};

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	char SendBuf[1024];
	char RecvBuf1[1024], RecvBuf2[61];
	inline static int BufLen = 1024;

	//Konstruktor
	NodeUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2){
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
		RecvAddr1.sin_family = AF_INET;
		RecvAddr1.sin_port = htons(Port1);
		RecvAddr1.sin_addr.s_addr = htonl(INADDR_ANY);

		//---------------------------------------------
		// Create a socket for sending data
		SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (SendSocket == INVALID_SOCKET) {
			std::cout << "socket niepowiod�o si� z b��dem: " << WSAGetLastError() << "\n";
			WSACleanup();
			return;
		}
		//---------------------------------------------
		// Set up the RecvAddr1 structure with the IP address
		// and the specified port number.
		RecvAddr2.sin_family = AF_INET;
		RecvAddr2.sin_port = htons(Port2);
		RecvAddr2.sin_addr.s_addr = inet_addr(IP.c_str());
	}

public:
	bool receive_text_protocol(TextProtocol& protocol) {
		std::cout << "Odbieranie komunikatu...\n";
		const int iResult = recvfrom(RecvSocket, RecvBuf1, BufLen, 0, (SOCKADDR *)& RecvAddr2, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy�anie niepowiod�o si� z b��dem: " << WSAGetLastError() << "\n";
			return false;
		}
		std::cout << "\nRecvBuf: " << RecvBuf1 << "\n\n";
		return true;
	}

	bool send_text_protocol(const TextProtocol& protocol, const std::bitset<8>& fields) {//pierwsze datagramy 61 dlugosc
		std::string sendStr = protocol.to_string(fields);

		std::cout << "D�ugo�� komunikatu: " << sendStr.length() << std::endl;

		const int iResult = sendto(SendSocket, sendStr.c_str(), sendStr.length(), 0, (SOCKADDR *)& RecvAddr1, sizeof(RecvAddr1));//61 dlugosci
		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy�anie niepowiod�o si� z b��dem: " << WSAGetLastError() << "\n";
			closesocket(SendSocket);
			WSACleanup();
			return false;
		}
		return true;
	}

	bool close_socket() const {
		const int iResult = closesocket(RecvSocket);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Zamykanie gniazdka niepowiod�o si� z b��dem: " << WSAGetLastError() << "\n";
			return false;
		}
		return true;
	}
};