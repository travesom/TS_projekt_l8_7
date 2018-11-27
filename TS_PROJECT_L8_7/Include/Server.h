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

class ServerUDP : public NodeUDP{
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
			std::cout << "socket niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
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
			std::cout << "Bindowanie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return;
		}
	};
	virtual ~ServerUDP() { WSACleanup(); };

	void send_text_protocol(const std::string& data) {
		strcpy_s(SendBuf, data.c_str());
		const int iResult = sendto(SendSocket, SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr2, sizeof(RecvAddr2));
		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy³anie komunikatów..." << WSAGetLastError() << "\n";
			closesocket(SendSocket);
			WSACleanup();
			return;
		}
		std::cout << SendBuf << std::endl;
	}

	void receive_text_protocol_1() {

		std::cout << "Odbieranie komunikatów...\n";
		const int iResult = recvfrom(RecvSocket, revbuf1, 61, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Odbieranie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return;
		}
		std::string temp(revbuf1); temp.resize(61);
		std::cout << "\nRecvBuf: " << temp << '\n';
		std::cout << "D³ugoœæ RecvBuf: " << sizeof(revbuf1);
		std::cout << "D³ugoœæ temp: " << temp.size() << "\n\n";
	}
	bool send_text_protocol_1(TextProtocol &d) {// wysyla klientow ID
		std::string temp;
		time_t rawtime;
		time(&rawtime);
		d.ID = randInt(10, 99);
		temp = HEAD_TIME; temp.append(std::to_string(d.time));
		temp.append(HEAD_ST);
		temp.append(std::string(1, d.ST));
		temp.append(HEAD_SN);
		temp.append(std::to_string(d.SN));
		temp.append(HEAD_ID);
		temp.append(std::to_string(d.ID));
		std::cout << "Treœæ string: " << temp << ", rozmiar stringa: " << temp.size() << std::endl;

		const int iResult = sendto(SendSocket, temp.c_str(), temp.size(), 0, (SOCKADDR *)& RecvAddr2, sizeof(RecvAddr2));
		if (iResult == SOCKET_ERROR) {
			std::cout << "sendto failed with error: " << WSAGetLastError() << "\n";
			closesocket(SendSocket);
			WSACleanup();
			return false;
		}
		return true;
	}
};
