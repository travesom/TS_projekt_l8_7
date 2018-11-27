// Server_Instance.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"
#include <iostream>
#include <cstdlib>  
#include <string>
#include <regex>

#pragma comment(lib, "Ws2_32.lib")// Link with ws2_32.lib



int main()
{
	system("chcp 1250");
	system("cls");

	const std::string IP = "127.0.0.1";
	const unsigned short Port1 = 27015;
	const unsigned short Port2 = 27014;

	ServerUDP server(IP, Port1, Port2);
	server.start_session();


	//-----------------------------------------------
	std::cout << "Zamykanie gniazdka...\n";
	if (!server.close_socket()) {
		std::cout << "Błąd zamykania gniazdka.\n";
	}
	//-----------------------------------------------
	std::cout << "Kończenie...\n";
	system("PAUSE");
	return 0;
}