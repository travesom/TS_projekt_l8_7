// UDP_serwv_29.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//
#pragma warning(disable:4996)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include "protokol.h"
#include <string>
#include <regex>
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
char nagOP[11]="operacja: ", nagST[9]="status: ", nagliczba1[13]="argument 1: ", nagliczba2[13] = "argument 2: ", nagNS[20]="Numer Sekwencyjny: ", nagID[16]="Identyfikator: ", nagIO[25]="Identyfikator operacji: ", nagtime[7]="czas: ";
bool check_OP(char ch) {
	if (ch == 'd')
		return true;


}
int main()
{	
	system("chcp 1250");
	char wybor;
	protokol d;
	time_t rawtime;
	time(&rawtime);

	//d.timeinfo = localtime(&rawtime);
	//printf("Current local time and date: %s", asctime(d.timeinfo));//asc to do stringa daje 
	std::string temp;
	d.time = (long int )rawtime;
	d.liczba1 = 32;
	d.liczba2 = 88;
	d.ST = 'p';
	d.NS=0;
	d.ID = 0;
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
	
	std::cout << sizeof(long int) << std::endl;;
	int BufLen = 1024;
	int RBufLen = 1024;
	std::cout << d.time << std::endl;
	std::cout << rawtime << std::endl;
	temp = nagtime;
	temp.append( std::to_string(d.time));
	temp.append(nagST);
	temp.append(std::string(1,d.ST));
	temp.append(nagNS);
	temp.append(std::to_string(d.NS));
	temp.append(nagID);
	temp.append(std::to_string(d.ID));
	strcpy_s(SendBuf, temp.c_str());

	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	//---------------------------------------------
	// Create a socket for sending data
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
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
		wprintf(L"socket failed with error %d\n", WSAGetLastError());
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
	wprintf(L"Nazwizywanie polaczenie\n");

	iResult = sendto(SendSocket,
		SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));//time||ST||NS||ID
	if (iResult == SOCKET_ERROR) {
		wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
		closesocket(SendSocket);
		WSACleanup();
		return 1;
	}
	//---------------------------------------------

	iResult=bind(RecvSocket, (SOCKADDR *)& RecvAddr2, sizeof(RecvAddr));
	if (iResult != 0) {
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
		return 1;
	}
	iResult=recvfrom(RecvSocket, RecvBuf, BufLen, 0, (SOCKADDR *)& RecvAddr2, &SenderAddrSize);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
	}


	std::cout << "wiadomowsc zwrotna " << RecvBuf << std::endl;
	std::cout <<"string" <<(std::string) RecvBuf << std::endl;;
	std::string match = "Identyfikator: ";
	std::size_t found = (((std::string) RecvBuf).find(match));
	d.ID = stoi(((std::string) RecvBuf).substr(found + 15));
	std::cout << d.ID << std::endl;;
	//-----------------------wysylanie info
	while (true) {
		do {
			std::cout << "wybierz opcje protokołu: " << std::endl << "jesli chcesz rozłoczyć wyslij r" << std::endl << " jesli chcesz cos obliczycyć wyslij o" << std::endl << "jesli chcesz zobaczyc historie wyslij h" << std::endl;
			std::cin >> wybor;
		} while (wybor != 'r');
		if (wybor == 'r'||wybor=='h'||wybor=='o') {
			time(&rawtime);
			d.time = (long int)rawtime;
			temp = nagtime;
			temp.append(std::to_string(d.time));
			temp.append(nagST);
			temp.append(std::string(1, 'r'));
			temp.append(nagNS);
			temp.append(std::to_string(0));
			temp.append(nagID);
			temp.append(std::to_string(d.ID));
			strcpy_s(SendBuf, temp.c_str());
			std::cout << SendBuf << std::endl;
			iResult = sendto(SendSocket,
				SendBuf, BufLen, 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));//time||ST||NS||ID
			if (iResult == SOCKET_ERROR) {
				wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
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
	wprintf(L"Finished sending. Closing socket.\n");
	iResult = closesocket(SendSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//---------------------------------------------
	// Clean up and quit.
	wprintf(L"Exiting.\n");
	WSACleanup();
	system("PAUSE");
	return 0;
}

// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln
