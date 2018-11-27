// TS_PROJECT_L8_7.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma warning(disable:4996)
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <iostream>
#include <string>
#include <regex>
#include "Protocol.h"
#include "Client.h"
#include <iomanip>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
long int get_time() {
	time_t rawtime;
	time(&rawtime);
	return (long int)rawtime;

};




int main()
{
	Sleep(1000); //Na końcu usunąć
	system("chcp 1250");
	system("cls");
	//char wybor;

	const std::string IP = "127.0.0.1";
	const unsigned short Port1 = 27014;
	const unsigned short Port2 = 27015;

	TextProtocol d;
	ClientUDP client(IP, Port1, Port2);

	bool stop = false;
	

	//Pętla nawiązywania sesji
	while (!stop) {
		if (!client.start_session()) {
			std::cout << "Błąd sesji.\n";
		}

		std::cout << "Czy chcesz rozpocząć ponownie sesję? (Y/N)";
		std::string answer;
		std::cin >> answer;
		if (answer == "Y" || answer == "y") { continue; }
		if (answer == "N" || answer == "n") { break; }
	}

	
	//---------------------------------------------
	// Clean up and quit.
	std::cout << "Kończenie...\n";
	WSACleanup();
	system("PAUSE");
	return 0;
}