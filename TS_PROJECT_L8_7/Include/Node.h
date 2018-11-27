#pragma once
#include <winsock2.h>
#include <iostream>
#include "Protocol.h"

#pragma comment(lib, "Ws2_32.lib")// Link with ws2_32.lib
class NodeUDP{
protected:
	WSADATA wsaData{};
	SOCKET SendSocket;
	SOCKET RecvSocket;
	sockaddr_in RecvAddr{};
	sockaddr_in RecvAddr2{};

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	char SendBuf[1024];
	char RecvBuf[1024], revbuf1[61];
	int BufLen = 1024;

	bool receive_text_protocol(TextProtocol&) {
		std::cout << "Odbieranie komunikatu...\n";
		const int iResult = recvfrom(RecvSocket, RecvBuf, BufLen, 0, (SOCKADDR *)& RecvAddr2, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Wysy³anie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return false;
		}
		std::cout << "\nRecvBuf: " << RecvBuf << "\n\n";
		return true;
	}
};