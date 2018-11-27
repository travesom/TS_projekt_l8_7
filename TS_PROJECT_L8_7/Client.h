#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"

int numer_operacji = 0;

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
		numer_operacji = 0;
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
				d.SN--;
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
					numer_operacji++;
					std::cout<<"numer operacji" <<numer_operacji <<"wynik wynosi: " << d.number1 << std::endl;
					return false;
				}
}
			if (wybor == 'o') {
				char wybor;
				bool test=true;
				std::string temp;
				d.ST = 'o';
				int c, arg2;
				do {
					std::cout << "Wyb�r operacji: \n"
						<< "- Je�li chcesz dodac wy�lij 'd'\n"
						<< "- Je�li chcesz odjac  wy�lij 'i' \n"
						<< "- Je�li chcesz pomnozyc  wy�lij 'm'\n "
						<< "- Je�li chcesz podzielic  wy�lij 's'\n";
					std::cout << "Wpisz sw�j wyb�r: ";
					std::cin >> wybor;
					
					if (wybor == 'd' || wybor == 'o' || wybor == 'm' || wybor == 's') {
						test = false;
					}
				} while (test);
					std::cout << "Podaj pierwszy argument " << std::endl;
					std::cin >> c;
				
				std::cout << "Podaj drugi argument " << std::endl;
				std::cin >> arg2;
				if (arg2 == 0) {
					std::cout << "Nie dzieli sie przez 0! podaj ponownie drugi argument " << std::endl;



				}
				d.number1 = c; d.number2 = arg2;
				d.OP = wybor;// tu jest na 1 litere tego wyboru pozniej trzeba to zrobic na stringa
				std::cout << d.OP << std::endl;
				d.SN = 3;
				if (!send_text_protocol(d, 0)) {
					std::cout << "B��d wysy�ania.\n";

				}
				d.SN --;
				if (!send_text_protocol(d, 1)) {
					std::cout << "B��d wysy�ania.\n";

				}
				
				d.SN--;
				if (!send_text_protocol(d, 2)) {
					std::cout << "B��d wysy�ania Silini.\n";

				}
				d.SN--;
				if (!send_text_protocol(d, 3)) {
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
					numer_operacji++;
					std::cout << "numer operacji" << numer_operacji << "wynik dzia�ania wynosi: " << d.number1 << std::endl;
					return false;
				}
			}
			if (wybor == 'h') {
				int wynik;
				std::string temp;
				int num;
				std::cout << "podaj numer operacji\n";
				std::cin>> num;
				d.OP_ID = num;
				d.ST = 'h';
				d.SN = 1;
				if (!send_text_protocol(d, 0)) {
					std::cout << "B��d wysy�ania.\n";

				}
				d.SN--;
				if (!send_text_protocol(d, 4)) {
					std::cout << "B��d wysy�ania Silini.\n";

				}
				Sleep(100);
				receive_text_protocol(temp);
				d.from_string(temp);
				wynik = d.number1;
				receive_text_protocol(temp);
				d.from_string(temp);
				receive_text_protocol(temp);
				d.from_string(temp);
				std::cout << "Operacja numer: " << d.OP_ID << " wynik: " << wynik << " argument1: " << d.number1 << " argument2: " << d.number2 << " typ dzia�anie: " << d.OP << std::endl;
				
				


			}








		}
		else  std::cout << "wprowadziles zle dane, podaj dane ponownie" << std::endl;
	};
};


