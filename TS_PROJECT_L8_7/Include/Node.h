#pragma once
#include "Protocol.h"
#include "Console.hpp"
#include <winsock2.h>
#include <iostream>
#include <Iphlpapi.h>
#include <set>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")


//Zwraca tablicê adresów IP z tablicy ARP, gdzie adresy podzielone s¹ na 4 czêœci
inline std::vector<std::vector<std::string>> GET_IP_TABLE_FRAGM() {
	//Zmienna do przechowania zawartoœci tablicy ARP
	PMIB_IPNETTABLE pIPNetTable = nullptr;
	//Pobieranie tablicy ARP
	{
		/*
			Fragment kodu bazowany na informacjach z poni¿szej strony:
			https://www.codeproject.com/Articles/5960/WebControls/?fid=32450&df=90&mpp=25&prof=True&sort=Position&view=Normal&spc=Relaxed&fr=51
		*/
		ULONG ulSize = 0;
		GetIpNetTable(pIPNetTable, &ulSize, TRUE);
		pIPNetTable = new MIB_IPNETTABLE[ulSize];
		if (nullptr != pIPNetTable) {
			GetIpNetTable(pIPNetTable, &ulSize, TRUE);
		}
	}

	//Pêtla parsuj¹ca uzyskane dane z tablicy ARP
	std::vector<std::string> IPv4_Correct;
	std::vector<std::vector<std::string>> IPv4_Table;
	for (unsigned int i = 0; i < pIPNetTable->dwNumEntries; i++) {
		struct in_addr addr;
		addr.s_addr = ntohl(static_cast<long>(pIPNetTable->table[i].dwAddr));
		std::string IPv4(inet_ntoa(addr));
		std::string temp;
		for (int j = IPv4.size() - 1; j >= 0; j--) {
			if (IPv4[j] != '.') { temp.insert(temp.begin(), IPv4[j]); }
			if (IPv4[j] == '.' || j == 0) {
				IPv4_Correct.push_back(temp);
				temp.clear();
			}
		}
		if (IPv4_Correct.size() == 4) {
			if (IPv4_Correct[0] != "224" && IPv4_Correct[0] != "255" &&
				IPv4_Correct[2] != "255" && IPv4_Correct[3] != "255") {
				IPv4_Table.push_back(IPv4_Correct);
			}
			IPv4_Correct.clear();
		}
	}
	return IPv4_Table;
}

//Zwraca tablicê adresów IP z tablicy ARP
inline std::set<std::string> GET_IP_TABLE() {
	//Tablica ca³ych adresów IP
	std::set<std::string> IPv4_Table;
	//Pêtla szukaj¹ca po¿¹danego IP
	for (const std::vector<std::string>& elem : GET_IP_TABLE_FRAGM()) {
		std::string result;
		for (unsigned int i = 0; i < elem.size(); i++) {
			result += elem[i];
			if (i < elem.size() - 1) { result += '.'; }
		}
		IPv4_Table.insert(result);
	}
	IPv4_Table.insert("127.0.0.1");
	return IPv4_Table;
}

static const unsigned int BufLen = 1024;
static const unsigned int id_message_size = 57;

class NodeUDP {
protected:
	WSADATA wsaData{};
	SOCKET nodeSocket;
	sockaddr_in otherAddr{};

	//Konstruktor
	NodeUDP(const unsigned short& Port1) {
		//Inicjalizacja WinSock
		const int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			std::cout << "WSAStartup failed with error " << iResult << "\n";
			system("pause");
			return;
		}

		//Tworzenie gniazdka do wysy³ania
		nodeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (nodeSocket == INVALID_SOCKET) {
			std::cout << "socket niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			WSACleanup();
			return;
		}

		//Adres drugiego wêz³a
		otherAddr.sin_family = AF_INET;
		otherAddr.sin_port = htons(Port1);
		otherAddr.sin_addr.s_addr = INADDR_ANY;
	}
	~NodeUDP(){ WSACleanup(); }

public:
	bool receive_text_protocol(std::string& received) {
		char recvBuffer[1024];
		int sendAddrLength = sizeof(otherAddr);

		//Wpisywanie dla pewnoœci NULL do tablicy znaków
		std::fill(std::begin(recvBuffer), std::end(recvBuffer), NULL);

		int iResult;
		for (int i = 10; i > 0; i--) {
			iResult = recvfrom(nodeSocket, recvBuffer, sizeof(recvBuffer), 0, reinterpret_cast<SOCKADDR *>(&otherAddr), &sendAddrLength);
			if (iResult != SOCKET_ERROR) { break; }
		}
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

	void send_text_protocol_to(const TextProtocol& protocol, const int& field, const std::string& address) {
		sockaddr_in addr{};

		//Adres drugiego wêz³a
		addr.sin_family = AF_INET;
		addr.sin_port = otherAddr.sin_port;
		addr.sin_addr.s_addr = inet_addr(address.c_str());

		//Dane do wys³ania
		const std::string sendStr = protocol.to_string(field);
		Sleep(100);
		const int iResult = sendto(nodeSocket, sendStr.c_str(), sendStr.length(), 0, reinterpret_cast<SOCKADDR *>(&addr), sizeof(addr));

		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy³anie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
		}
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