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

	//Konstruktor
	NodeUDP(const u_long& IP, const unsigned short& Port1) {
		//Inicjalizacja WinSock
		const int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			std::cout << "WSAStartup failed with error " << iResult << "\n";
			return;
		}

		//Tworzenie gniazdka do wysy³ania
		nodeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (nodeSocket == INVALID_SOCKET) {
			std::cout << "socket niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			WSACleanup();
			return;
		}

		//Adres wêz³a
		nodeAddr.sin_family = AF_INET;
		nodeAddr.sin_port = htons(Port1);
		nodeAddr.sin_addr.s_addr = INADDR_ANY;

		//Adres nadawcy/odbiorcy
		otherAddr.sin_family = AF_INET;
		otherAddr.sin_port = htons(Port1);
		otherAddr.sin_addr.s_addr = IP;
	}
	~NodeUDP(){ WSACleanup(); }

public:
	bool receive_text_protocol(std::string& received) {
		char recvBuffer[1024];
		int sendAddrLength = sizeof(otherAddr);

		//Wpisywanie dla pewnoœci NULL do tablicy znaków
		std::fill(std::begin(recvBuffer), std::end(recvBuffer), NULL);

		const int iResult = recvfrom(nodeSocket, recvBuffer, sizeof(recvBuffer), 0, reinterpret_cast<SOCKADDR *>(&otherAddr), &sendAddrLength);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Odbieranie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return false;
		}

		//Przepisywanie zawartoœci bufora do string'a
		std::string result;
		for (unsigned int i = 0; i < 1024; i++) {
			//NULL oznacza koniec stringa
			if (recvBuffer[i] != NULL) {
				result.push_back(recvBuffer[i]);
			}
			else { 
				//NULL termination
				result.push_back(recvBuffer[i]);
				break;
			}
		}

		received = result;
		return true;
	}

	bool send_text_protocol(const TextProtocol& protocol, const int& field) {
		//Dane do wys³ania
		const std::string sendStr = protocol.to_string(field);

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
	std::vector<TextProtocol> receive_messages() {
		//Kontener na otrzymanie komunikaty
		std::vector<TextProtocol> receivedMessages;

		//Pêtla odbierania danych
		while (true) {
			std::string received;
			receive_text_protocol(received);
			const TextProtocol receivedProtocol(received);
			receivedMessages.push_back(receivedProtocol);

			//Odbieranie koñczy siê przy natrafieniu na numer sekwencyjny 0
			if (receivedProtocol.sequenceNumber == 0) { break; }
		}
		return receivedMessages;
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