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

	//void send_text_protocol(const std::string& data) {
	//	strcpy_s(SendBuf, data.c_str());
	//	const int iResult = sendto(SendSocket, SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr2, sizeof(RecvAddr2));
	//	if (iResult == SOCKET_ERROR) {
	//		std::cout << "Wysy³anie komunikatów..." << WSAGetLastError() << "\n";
	//		closesocket(SendSocket);
	//		WSACleanup();
	//		return;
	//	}
	//	std::cout << SendBuf << std::endl;
	//}

	void receive_text_protocol_1() {

		std::cout << "Odbieranie komunikatów...\n";
		const int iResult = recvfrom(RecvSocket, RecvBuf2, id_message_size, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Odbieranie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return;
		}
		std::string temp(RecvBuf2); temp.resize(id_message_size);
		std::cout << "\nRecvBuf: " << temp << '\n';
		std::cout << "D³ugoœæ RecvBuf: " << sizeof(RecvBuf2);
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
