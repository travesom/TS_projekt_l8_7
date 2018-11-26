// Server_Instance.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"
#include <winsock2.h>
#include <iostream>
#include <cstdlib>  
#include <string>
#include <regex>

#pragma comment(lib, "Ws2_32.lib")// Link with ws2_32.lib

int main()
{
	system("chcp 1250");
	system("cls");
	time_t rawtime;
	int iResult = 0;

	const std::string IP = "127.0.0.1";
	const unsigned short Port1 = 27015;
	const unsigned short Port2 = 27014;

	ServerUDP server(IP, Port1, Port2);

	//-----------------------------------------------
	// Call the recvfrom function to receive datagrams
	// on the bound socket.


	time(&rawtime);
	TextProtocol d('p', 0, randInt(1, 100), (long int)rawtime);

	//Wysłanie protokołu
	server.send_text_protocol(d.to_string(PROT_ID));


	//-----------------------------------------------
	// Close the socket when finished receiving datagrams
	std::cout << "Finished receiving. Closing socket.\n";
	iResult = closesocket(server.RecvSocket);
	if (iResult == SOCKET_ERROR) {
		std::cout << "closesocket failed with error " << WSAGetLastError() << "\n";
		return 1;
	}

	//-----------------------------------------------
	// Clean up and exit.
	std::cout << "Exiting...\n";
	system("PAUSE");
	return 0;
}