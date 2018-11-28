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
	SOCKET nodeSocket;
	sockaddr_in nodeAddr{};
	sockaddr_in otherAddr{};
	char buffer[1024];


	//Konstruktor
	NodeUDP(const u_long& IP, const unsigned short& Port1) {
		// Initialize Winsock
		const int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			std::cout << "WSAStartup failed with error " << iResult << "\n";
			return;
		}

		//---------------------------------------------
		// Create a socket for sending data
		nodeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (nodeSocket == INVALID_SOCKET) {
			std::cout << "socket niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			WSACleanup();
			return;
		}
		//---------------------------------------------
		// Set up the nodeAddr structure with the IP address
		// and the specified port number.
		otherAddr.sin_family = AF_INET;
		otherAddr.sin_port = htons(Port1);
		otherAddr.sin_addr.s_addr = IP;

		nodeAddr.sin_family = AF_INET;
		nodeAddr.sin_port = htons(Port1);
		nodeAddr.sin_addr.s_addr = INADDR_ANY;
	}

public:
	bool receive_text_protocol(std::string& received) {
		for (unsigned int i = 0; i < 1024; i++) {
			buffer[i] = NULL;
		}

		//std::cout << "Odbieranie komunikatu...\n";


		//::cout << "Address: " << inet_ntoa(nodeAddr.sin_addr) << '\n';
		//std::cout << "Port: " << nodeAddr.sin_port << '\n';

		int sendAddrLength = sizeof(otherAddr);
		const int iResult = recvfrom(nodeSocket, buffer, sizeof(buffer), 0, reinterpret_cast<SOCKADDR *>(&otherAddr), &sendAddrLength);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Odbieranie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return false;
		}
		std::string result;
		//std::cout << "\nbufor: " << buffer << "\n";
		for (unsigned int i = 0; i < 1024; i++) {
			if (buffer[i] != NULL) {
				result.push_back(buffer[i]);
			}
			else { break; }
		}

		//NULL termination
		result.push_back(NULL);

		received = result;
		//std::cout << "\nResult: " << result << "\n";
		//std::cout << "D³ugoœæ: " << result.length() << "\n";

		return true;
	}


#define SEND_NO_ADDITIONAL -1
#define SEND_SEQUENTIAL_NUMBER 0
#define SEND_NUMBER 1
#define SEND_CALCULATION_ID 2
#define SEND_STATUS 3

	bool send_text_protocol(const TextProtocol& protocol, const int& field) {
		//std::cout << "\nWysy³anie komunikatu...\n";
		std::string sendStr = protocol.to_string(field);

		//std::cout << "Komunikat: " << sendStr << "\nD³ugoœæ komunikatu: " << sendStr.length() << std::endl;

		const int iResult = sendto(nodeSocket, sendStr.c_str(), sendStr.length(), 0, reinterpret_cast<SOCKADDR *>(&otherAddr), sizeof(otherAddr));
		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy³anie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			closesocket(nodeSocket);
			WSACleanup();
			return false;
		}
		return true;
	}

	//Zwraca wektor z wszystkimi komunikatami od operacji
	std::vector<TextProtocol> receive_parts() {
		std::vector<TextProtocol> receivedProtocols;
		std::string received;

		//Pêtla pozyskiwania danych
		while (true) {
			//Odebranie komunikatu z numerem sekwencyjnym
			receive_text_protocol(received);
			const TextProtocol sequenceProtocol(received);
			const unsigned int sequenceNumber = sequenceProtocol.sequenceNumber;

			if (sequenceNumber == 0) { break; }

			//Odebranie komunikatu z innymi danymi
			receive_text_protocol(received);
			const TextProtocol partProtocol(received);
			receivedProtocols.push_back(partProtocol);
		}
		return receivedProtocols;
	}

	bool send_sequence_number(const unsigned int& sessionId_, const unsigned int& sequenceNumber) {
		//Wys³anie numeru sekwencyjnego
		TextProtocol sequenceProtocol(GET_CURRENT_TIME(), sessionId_, "NUMER_SEKWENCYJNY");
		sequenceProtocol.sequenceNumber = sequenceNumber;
		return send_text_protocol(sequenceProtocol, SEND_SEQUENTIAL_NUMBER);
	}


	bool close_socket() const {
		const int iResult = closesocket(nodeSocket);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Zamykanie gniazdka niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return false;
		}
		return true;
	}
};