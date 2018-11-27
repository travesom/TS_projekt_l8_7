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
	const unsigned short Port1 = 27014;
	const unsigned short Port2 = 27015;

	ServerUDP server(IP, Port1, Port2);

	//-----------------------------------------------
	// Call the recvfrom function to receive datagrams
	// on the bound socket.


	time(&rawtime);
	TextProtocol d('p', 0, randInt(10, 99), (long int)rawtime);

	//Wysłanie protokołu
	server.receive_text_protocol_1();
	if (!server.send_text_protocol_1(d)) {
		std::cout << "Błąd wysyłania.\n";
	}


	//-----------------------------------------------
	// Close the socket when finished receiving datagrams
	std::cout << "Skończono odbieranie. Zamykanie gniazdka...\n";
	if(!server.close_socket()){
		std::cout << "Błąd zamykania gniazdka.\n";
	}
	//-----------------------------------------------
	// Clean up and exit.
	std::cout << "Kończenie...\n";
	system("PAUSE");
	return 0;
}