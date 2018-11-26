// TS_PROJECT_L8_7.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma warning(disable:4996)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Protocol.h"
#include <winsock2.h>
#include <iostream>
#include <string>
#include <regex>
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

bool check_OP(char ch) {
	if (ch == 'd')
		return true;
}
int main()
{
	system("chcp 1250");
	char wybor;
	time_t rawtime;
	time(&rawtime);

	//d.timeinfo = localtime(&rawtime);
	//printf("Current local time and date: %s", asctime(d.timeinfo));//asc to do stringa daje 
	std::string temp;



	//temp = std::to_string(d.liczba1);
	int iResult;
	WSADATA wsaData;

	SOCKET SendSocket = INVALID_SOCKET;
	SOCKET RecvSocket;
	sockaddr_in RecvAddr;
	sockaddr_in RecvAddr2;
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	unsigned short Port = 27015;
	unsigned short Port2 = 27014;

	char SendBuf[1024];
	char RecvBuf[1024];

	std::cout << sizeof(long int) << "\n";;
	int BufLen = 1024;
	int RBufLen = 1024;

	TextProtocol d('p', 32, 88, 0, 0, (long int)rawtime);

	std::cout << d.time << "\n";
	std::cout << rawtime << "\n";

	strcpy_s(SendBuf, d.to_string(PROT_ID).c_str());

	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		std::cout << "WSAStartup failed with error: " << iResult << "\n";
		return 1;
	}

	//---------------------------------------------
	// Create a socket for sending data
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET) {
		std::cout << "socket failed with error: " << WSAGetLastError() << "\n";
		WSACleanup();
		return 1;
	}
	//---------------------------------------------
	// Set up the RecvAddr structure with the IP address of
	// the receiver (in this example case "127.0.0.1")
	// and the specified port number.
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//-----------------------------------------------
	// Create a receiver socket to receive datagrams
	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (RecvSocket == INVALID_SOCKET) {
		std::cout << "socket failed with error " << WSAGetLastError() << "\n";
		return 1;
	}
	//-----------------------------------------------
	// Bind the socket to any address and the specified port.
	RecvAddr2.sin_family = AF_INET;
	RecvAddr2.sin_port = htons(Port2);
	RecvAddr2.sin_addr.s_addr = htonl(INADDR_ANY);

	//---------------------------------------------

	//---------------------------------------------
	// Send a datagram to the receiver
	std::cout << "Nawiązywanie połączenia\n";

	iResult = sendto(SendSocket,
		SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));//time||ST||NS||ID
	if (iResult == SOCKET_ERROR) {
		std::cout << "sendto failed with error: " << WSAGetLastError() << "\n";
		closesocket(SendSocket);
		WSACleanup();
		return 1;
	}
	//---------------------------------------------

	iResult = bind(RecvSocket, (SOCKADDR *)& RecvAddr2, sizeof(RecvAddr));
	if (iResult != 0) {
		std::cout << "bind failed with error " << WSAGetLastError() << "\n";
		return 1;
	}
	iResult = recvfrom(RecvSocket, RecvBuf, BufLen, 0, (SOCKADDR *)& RecvAddr2, &SenderAddrSize);
	if (iResult == SOCKET_ERROR) {
		std::cout << "recvfrom failed with error " << WSAGetLastError() << "\n";
	}


	std::cout << "Wiadomowść zwrotna " << RecvBuf << "\n";
	std::string RecvBufStr = std::string(RecvBuf);
	std::cout << "String: " << RecvBufStr << "\n";
	std::string match = HEAD_ID;
	std::size_t found = (RecvBufStr.find(match));
	d.ID = stoi(RecvBufStr.substr(found + 15));
	std::cout << d.ID << "\n";;
	//-----------------------wysylanie info
	while (true) {
		do {
			std::cout << "Wybierz opcje protokołu: " << "\n" << "jeśli chcesz rozłączyć wyślij r" << "\n" << " jeśli chcesz coś obliczyć wyslij o" << "\n" << "jeśli chcesz zobaczyć historię wyślij h" << "\n";
			std::cin >> wybor;
		} while (wybor != 'r');
		if (wybor == 'r' || wybor == 'h' || wybor == 'o') {
			time(&rawtime);
			d.time = (long int)rawtime;

			strcpy_s(SendBuf, d.to_string(PROT_ID).c_str());
			std::cout << SendBuf << "\n";
			iResult = sendto(SendSocket,
				SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));//time||ST||NS||ID
			if (iResult == SOCKET_ERROR) {
				std::cout << "sendto failed with error: " << WSAGetLastError() << "\n";
				closesocket(SendSocket);
				WSACleanup();
				return 1;
			}



		}
		else if (wybor == 'o') {



			break;
		}
		else if (wybor == 'h') {

			break;

		}



	}



	// When the application is finished sending, close the socket.
	std::cout << "Finished sending. Closing socket.\n";
	iResult = closesocket(SendSocket);
	if (iResult == SOCKET_ERROR) {
		std::cout << "closesocket failed with error: " << WSAGetLastError() << "\n";
		WSACleanup();
		return 1;
	}
	//---------------------------------------------
	// Clean up and quit.
	std::cout << "Exiting.\n";
	WSACleanup();
	system("PAUSE");
	return 0;
}