﻿// Server_Instance.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
	int iResult = 0;

	const std::string IP = "127.0.0.1";
	const unsigned short Port1 = 27015;
	const unsigned short Port2 = 27014;

	ServerUDP server(IP, Port1, Port2);

	server.receive_text_protocol_1();

	//Wysłanie protokołu
	std::string temp;
	time_t rawTime;
	time(&rawTime);

	const TextProtocol d('p', 0, randInt(10, 99), static_cast<long int>(rawTime));

	if (!server.send_text_protocol(d, 0)) {
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