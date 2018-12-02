#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"

#pragma comment(lib, "Ws2_32.lib")// Link with ws2_32.lib

int main()
{
	system("chcp 1250");
	system("cls");

	const unsigned short Port1 = 27272;

	ServerUDP server(Port1);
	while (true) {
		if(!server.start_session()){
			std::cout << "\nSesja przerwana!\n";
		}
	}
}