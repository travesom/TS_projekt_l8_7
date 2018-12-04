#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"

#pragma comment(lib, "Ws2_32.lib")// Link with ws2_32.lib

int main()
{
	system("chcp 1250");
	CONSOLE_MANIP::clear_console();

	ServerUDP server(PORT_TO_SET);
	while (true) {
		if(!server.start_session()){
			sync_cout << "\nSesja przerwana!\n";
		}
	}
}