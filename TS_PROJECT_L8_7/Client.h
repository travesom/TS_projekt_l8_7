#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"


class ClientUDP : public NodeUDP {
	unsigned int sessionId = 0;

public:
	ClientUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) :NodeUDP(IP, Port1, Port2) {};
	virtual ~ClientUDP() { WSACleanup(); };

	bool start_session() {
		TextProtocol d('p', 0, 0, GET_CURRENT_TIME()); d.ST = 'p';
		std::string temp = HEAD_ID;
		std::cout <<"Wielko��: "<< temp.size() << '/n';
		//��danie o rozpocz�cie sesji
		if (!send_text_protocol(d, 0)) {
			std::cout << "B��d wysy�ania.\n";
			return false;
		}
		//Odbieranie id
		std::string received;
		receive_text_protocol(received);
		d.from_string(received);
		sessionId = d.ID;
		while (!choose_status(d)) {}
		

		return true;
	}
	bool choose_status(TextProtocol& d) {
		char wybor;
		std::cout << "Wyb�r opcji protoko�u: \n"
			<< "- Je�li chcesz roz��czy� wy�lij 'r'\n"
			<< "- Je�li chcesz co� obliczy� z 2 argumentami wy�lij 'o'\n"
			<< "- Je�li chcesz  obliczy� silnie wy�lij 's'\n "
			<< "- je�li chcesz zobaczy� histori� wy�lij 'h'\n";
		std::cout << "Wpisz sw�j wyb�r: ";
		std::cin >> wybor;
		if (wybor == 'r' || wybor == 'o' || wybor == 'h'||wybor=='s') {//
			if (wybor == 'r') {

				d.SN = 0;
				d.ST = 'r';
				if (!send_text_protocol(d, 0)) {
					std::cout << "B��d wysy�ania.\n";
					
				}

				return true;//rozlacza

			}
			if (wybor == 's') {	
				std::string temp;
				d.ST = 's';
				int c;
				std::cout << "podaj argument  silni" << std::endl;
				std::cin >> c;
				d.SN = 1;
				if (!send_text_protocol(d, 0)) {
					std::cout << "B��d wysy�ania.\n";

				}
				d.number1 = c;
				d.SN = 0;
				if (!send_text_protocol(d, 2)) {
					std::cout << "B��d wysy�ania Silini.\n";
					
				}
				Sleep(100);
				receive_text_protocol(temp);
				d.from_string(temp);
				if (d.ST == 'e') {
					std::cout << "error za du�y argument \n";
					return false;
				}
				else
				{ 
					receive_text_protocol(temp);
					d.from_string(temp);
					std::cout << "wynik wynosi: " << d.number1 << std::endl;
					return false;
				}





			}








		}
		else  std::cout << "wprowadziles zle dane, podaj dane ponownie" << std::endl;
	};
};


