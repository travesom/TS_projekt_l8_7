#pragma once
#include "Protocol.h"
#include "Console.hpp"
#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")// Link with ws2_32.lib

static const unsigned int BufLen = 1024;
static const unsigned int id_message_size = 57;

class NodeUDP {
protected:
	WSADATA wsaData{};
	SOCKET SendSocket;
	SOCKET RecvSocket;
	sockaddr_in RecvAddr1{};
	sockaddr_in SendAddr1{};
	char buffer[1024];


	//Konstruktor
	NodeUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) {
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
			std::cout << "socket niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			WSACleanup();
			return;
		}
		//---------------------------------------------
		// Set up the RecvAddr1 structure with the IP address
		// and the specified port number.
		SendAddr1.sin_family = AF_INET;
		SendAddr1.sin_port = htons(Port2);
		SendAddr1.sin_addr.s_addr = inet_addr(IP.c_str());

		iResult = bind(RecvSocket, (SOCKADDR *)& RecvAddr1, sizeof(RecvAddr1));
		if (iResult != 0) {
			std::cout << "Bindowanie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return;
		}
	}

public:
	bool receive_text_protocol(std::string& protocol) {
		for(unsigned int i = 0; i < 1024; i++){
			buffer[i] = NULL;
		}

		std::cout << "Odbieranie komunikatu...\n";


		//std::cout << "Address: " << inet_ntoa(RecvAddr1.sin_addr) << '\n';
		//std::cout << "Port: " << RecvAddr1.sin_port << '\n';

		int RecvAddrSize = sizeof(RecvAddr1);
		const int iResult = recvfrom(RecvSocket, buffer, sizeof(buffer), 0, reinterpret_cast<SOCKADDR *>(&RecvAddr1), &RecvAddrSize);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy³anie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return false;
		}
		std::string result;
		std::cout << "\nbufor: " << buffer		 << "\n";
		for (unsigned int i = 0; i < 1024; i++) {
			if (buffer[i] != NULL) {
				result.push_back(buffer[i]);
			}
			else { break; }
		}

		//NULL termination
		result.push_back(NULL);

		//Spacja na koñcu u³atwi deserializacjê
		result += ' ';

		protocol = result;
		//std::cout << "\nResult: " << result << "\n";
		//std::cout << "D³ugoœæ: " << result.length() << "\n";

		return true;
	}

	/**
	* Znaczenia poszczególnych wartoœci int field: \n
	* 0 - pole ST \n
	* 1 - pole OP \n
	* 2 - pole number \n
	* 3 - pole OP_ID
	*/
	bool send_text_protocol(const TextProtocol& protocol, const int& field) {
		//std::cout << "\nWysy³anie komunikatu...\n";
		std::string sendStr = protocol.to_string(field);

		//std::cout << "Komunikat: " << sendStr << "\nD³ugoœæ komunikatu: " << sendStr.length() << std::endl;

		const int iResult = sendto(SendSocket, sendStr.c_str(), sendStr.length(), 0, (SOCKADDR *)& SendAddr1, sizeof(SendAddr1));
		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy³anie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			closesocket(SendSocket);
			WSACleanup();
			return false;
		}
		return true;
	}

	bool close_socket() const {
		const int iResult = closesocket(RecvSocket);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Zamykanie gniazdka niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return false;
		}
		return true;
	}
};