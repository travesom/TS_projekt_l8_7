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

void choose_status(TextProtocol& d, ClientUDP& klient) {
	char wybor;
	std::cout << "Wybór opcji protokołu: \n"
		<< "- Jeśli chcesz rozłączyć wyślij 'r'\n"
		<< "- Jeśli chcesz coś obliczyć wyślij 'o'\n"
		<< "- jeśli chcesz zobaczyć historię wyślij 'h'\n";
	std::cout << "Wpisz swój wybór: ";
	std::cin >> wybor;
	if (wybor == 'r' || wybor == 'o' || wybor == 'h') {
		if (wybor == 'r') {
			std::string temp, temp2;
			d.SN = 1;
			d.ST = 'r';
			
			std::cout << "dlugosc komunikatu rozlączenia: " << temp.length() << std::endl;
			temp2.append(HEAD_LENGTH);
			temp2.append(std::string(1, d.ST));
			temp2.append(HEAD_SN);
			temp2.append(std::to_string(d.SN));
			temp2.append(HEAD_ID);
			temp2.append(std::to_string(d.ID));
			std::cout << "dlugosc komunikatu informujacego odlugosci: " << temp2.length() << std::endl;







		}




	}
	else { std::cout << "wprowadziles zle dane, podaj dane ponownie" << std::endl; };






}


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

	while (true) {
		choose_status(d, client);
		if (d.OP == 'r') { break; }
	}
	//---------------------------------------------
	// Clean up and quit.
	std::cout << "Kończenie...\n";
	WSACleanup();
	system("PAUSE");
	return 0;
}