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
	std::cout << "wybierz opcje protokołu: " << std::endl 
		<< "jesli chcesz rozłączyć wyslij r" << std::endl 
		<< " jesli chcesz cos obliczycyć wyslij o" << std::endl 
		<< "jesli chcesz zobaczyc historie wyslij h" << std::endl;
	std::cin >> wybor;
	if (wybor == 'r' || wybor == 'o' || wybor == 'h') {
		if (wybor = 'r') {
			std::string temp,temp2;
			d.SN = 1;
			d.ST = 'r';
			d.time = get_time();
			temp = HEAD_TIME; temp.append(std::to_string(d.time));
			temp.append(HEAD_ST);
			temp.append(std::string(1, d.ST));
			temp.append(HEAD_SN);
			temp.append(std::to_string(d.SN));
			temp.append(HEAD_ID);
			temp.append(std::to_string(d.ID));
			d.time = get_time();
			d.Lenght = temp.length();
			std::cout << "dlugosc komunikatu rozlączenia: " << temp.length() <<std::endl;
			temp2 = HEAD_TIME; temp.append(std::to_string(d.time));
			temp2.append(HEAD_LENGTH);
			temp2.append(std::string(1, d.ST));
			temp2.append(HEAD_SN);
			temp2.append(std::to_string(d.SN));
			temp2.append(HEAD_ID);
			temp2.append(std::to_string(d.ID));
			std::cout << "dlugosc komunikatu informujacego odlugosci: " << temp2.length() << std::endl;
			



	 
		
		
		}
	
	
	
	
	 }
	else { std::cout << "wprowadziles zle dane, podaj dane ponownie" << std :: endl; };






}
int main()
{
	system("chcp 1250");
	char wybor;
	time_t rawtime;
	time(&rawtime);

	
	//printf("Current local time and date: %s", asctime(d.timeinfo));//asc to do stringa daje 
	std::string temp;



	//temp = std::to_string(d.liczba1);
	

	unsigned short Port = 27014;
	unsigned short Port2 = 27015;
	const std::string IP = "127.0.0.1";
	char SendBuf[1024];
	char RecvBuf[1024];


	int BufLen = 1024;
	int RBufLen = 1024;
	ClientUDP klient(IP, Port, Port2);
	TextProtocol d('p', 32, 88, 0, 0, (long int)rawtime);

	std::cout << d.time << "\n";
	std::cout << rawtime << "\n";

	klient.send_text_protocol_1();
	klient.receive_text_protocol_1(d);
	while (true) {
		choose_status(d,klient);
		if (d.OP = 'r')break;







	}
	//---------------------------------------------
	// Clean up and quit.
	std::cout << "Exiting.\n";
	WSACleanup();
	system("PAUSE");
	return 0;
}