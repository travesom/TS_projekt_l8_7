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
	ClientUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) {
		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			std::cout << "WSAStartup failed with error " << iResult << "\n";
			return;
		}
		//-----------------------------------------------
		// Create a receiver socket to receive datagrams
		SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (SendSocket == INVALID_SOCKET) {
			wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return ;
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
			wprintf(L"socket failed with error %d\n", WSAGetLastError());
			return ;
		}
		//---------------------------------------------
		// Set up the RecvAddr structure with the IP address
		// and the specified port number.
		RecvAddr2.sin_family = AF_INET;
		RecvAddr2.sin_port = htons(Port2);
		RecvAddr2.sin_addr.s_addr = htonl(INADDR_ANY);

		iResult = bind(RecvSocket, (SOCKADDR *)& RecvAddr2, sizeof(RecvAddr));
		if (iResult != 0) {
			wprintf(L"bind failed with error %d\n", WSAGetLastError());
			return ;
		}
	};
	virtual ~ClientUDP() { WSACleanup(); };

	void send_text_protocol(const std::string& data) {
		strcpy_s(SendBuf, data.c_str());
		const int iResult = sendto(SendSocket, SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
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
		const int iResult=recvfrom(RecvSocket, RecvBuf, BufLen, 0, (SOCKADDR *)& RecvAddr2, &SenderAddrSize);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
	}
		std::cout << "\nRecvBuf is: " << RecvBuf << "\n\n";
	}
	void send_text_protocol_1() {//pierwsze datagramy 61 dlugosc
		std::string temp;
		time_t rawtime;
		time(&rawtime);
		TextProtocol d; d.time = (long int)rawtime; d.ST = 'p'; d.SN = 0; d.ID = 0;
		temp = HEAD_TIME; temp.append(std::to_string(d.time));
		temp.append(HEAD_ST);
		temp.append(std::string(1, d.ST));
		temp.append(HEAD_SN);
		temp.append(std::to_string(d.SN));
		temp.append(HEAD_ID);
		temp.append(std::to_string(d.ID));
		std::cout << "dlugosc 1 komunikatu: " << temp.length() << std::endl;

		const int iResult = sendto(SendSocket, temp.c_str(), temp.length(), 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));//61 dlugosci
		if (iResult == SOCKET_ERROR) {
			std::cout << "sendto failed with error: " << WSAGetLastError() << "\n";
			closesocket(SendSocket);
			WSACleanup();
			return;
		}
		
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

