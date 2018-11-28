#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"

int numer_operacji = 0;

class ClientUDP : public NodeUDP {
public:
	unsigned int sessionId = 0;

	ClientUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) :NodeUDP(IP, Port1, Port2) {};
	virtual ~ClientUDP() { WSACleanup(); };

	bool start_session() {
		TextProtocol d("Rozpoczecie", 0, GET_CURRENT_TIME());
		//¯¹danie o rozpoczêcie sesji
		if (!send_text_protocol(d, -1)) {
			std::cout << "B³¹d wysy³ania.\n";
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
		const unsigned int boxWidth = 80;
		const unsigned int boxHeight = 20;

		const std::string actionChoice = "Wybór akcji:";
		std::string disconnect = " Zakoñczenie sesji.";
		std::string calculate = " Obliczenie.";
		std::string history = " Historia.";
		std::string sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);
		int choice = 1;

		goto pomin_petle; //Tymczasowe pominiêcie ³adnego menu


			//Wyœwietlanie obramowania, id sesji i tekstu odnoœnie wyboru
		CONSOLE_MANIP::clear_console();
		CONSOLE_MANIP::show_console_cursor(false);
		CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
		CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);
		CONSOLE_MANIP::print_text(2, 2, actionChoice);

		while (true) {
			while (true) {
				//Dodawanie wskaŸnika wybranej opcji
				if (choice == 1) {
					disconnect[0] = '>';
					calculate[0] = ' ';
					history[0] = ' ';
				}
				else if (choice == 2) {
					disconnect[0] = ' ';
					calculate[0] = '>';
					history[0] = ' ';
				}
				else if (choice == 3) {
					disconnect[0] = ' ';
					calculate[0] = ' ';
					history[0] = '>';
				}
				else {
					disconnect[0] = ' ';
					calculate[0] = ' ';
					history[0] = ' ';
				}

				//Wyœwietlanie opcji
				CONSOLE_MANIP::print_text(2, 4, disconnect);
				CONSOLE_MANIP::print_text(2, 5, calculate);
				CONSOLE_MANIP::print_text(2, 6, history);

				//Czyszczynie bufora wejœcia, aby po wduszeniu przycisku,
				// jego akcja nie zosta³a wielokrotnie wykonana
				CONSOLE_MANIP::clear_console_input_buffer();

				//Sprawdzanie naciœniêtych klawiszy
				if (CONSOLE_MANIP::check_arrow("UP") && choice > 1) { choice--; }
				else if (CONSOLE_MANIP::check_arrow("DOWN") && choice < 3) { choice++; }
				else if (CONSOLE_MANIP::check_enter()) { break; }
			}

			//Zakoñczenie sesji
			if (choice == 1) {
				d.SN = 0;
				d.ST = 'r';
				if (!send_text_protocol(d, 0)) {
					std::cout << "B³¹d wysy³ania.\n";
				}
				return true;
			}

			//Obliczenia
			else if (choice == 2) {
				int choiceCalc = 1;
				std::string goBack = " Powrót.";
				std::string addition = " Dodanie dwóch liczb.";
				std::string substraction = " Odjêcie dwóch liczb.";
				std::string multiplication = " Mno¿enie dwóch liczb.";
				std::string division = " Dzielenie dwóch liczb.";
				std::string strong = " Silnia z liczby.";

				//Wyœwietlanie obramowania, id sesji i tekstu odnoœnie wyboru
				CONSOLE_MANIP::clear_console();
				CONSOLE_MANIP::show_console_cursor(false);
				CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
				CONSOLE_MANIP::print_text(80 - sessionIdInfo.length() - 2, 1, sessionIdInfo);
				CONSOLE_MANIP::print_text(2, 2, actionChoice);

				while (true) {
					//Dodawanie wskaŸnika wybranej opcji
					goBack[0] = ' ';
					addition[0] = ' ';
					substraction[0] = ' ';
					multiplication[0] = ' ';
					division[0] = ' ';
					strong[0] = ' ';
					if (choiceCalc == 1) { goBack[0] = '>'; }
					else if (choiceCalc == 2) { addition[0] = '>'; }
					else if (choiceCalc == 3) { substraction[0] = '>'; }
					else if (choiceCalc == 4) { multiplication[0] = '>'; }
					else if (choiceCalc == 5) { division[0] = '>'; }
					else if (choiceCalc == 6) { strong[0] = '>'; }

					//Wyœwietlanie opcji
					CONSOLE_MANIP::print_text(2, 4, goBack);
					CONSOLE_MANIP::print_text(2, 5, addition);
					CONSOLE_MANIP::print_text(2, 6, substraction);
					CONSOLE_MANIP::print_text(2, 7, multiplication);
					CONSOLE_MANIP::print_text(2, 8, division);
					CONSOLE_MANIP::print_text(2, 9, strong);

					//Czyszczynie bufora wejœcia, aby po wduszeniu przycisku,
					// jego akcja nie zosta³a wielokrotnie wykonana
					CONSOLE_MANIP::clear_console_input_buffer();

					//Sprawdzanie naciœniêtych klawiszy
					if (CONSOLE_MANIP::check_arrow("UP") && choiceCalc > 1) { choiceCalc--; }
					else if (CONSOLE_MANIP::check_arrow("DOWN") && choiceCalc < 6) { choiceCalc++; }
					else if (CONSOLE_MANIP::check_enter()) { break; }
				}
				//Przejœcie do wykonywania wybranego dzia³ania
				//Powrót
				if (choiceCalc == 1) { break; }
				//Dodawanie
				else if (choiceCalc == 2) {

				}
				//Odejmowanie
				else if (choiceCalc == 3) {

				}
				//Mno¿enie
				else if (choiceCalc == 4) {

				}
				//Dzielenie
				else if (choiceCalc == 5) {
					std::string input1;
					std::cout << "Podaj pierwszy argument: ";
					CONSOLE_MANIP::input_string_digits(input1, 10);

					std::string input2;
					std::cout << "Podaj drugi argument: ";
					while (true) {
						CONSOLE_MANIP::input_string_int_number(input2, 10);
						if (std::stoi(input2) == 0) {
							std::cout << "Dzielnik nie mo¿e byæ 0!\n";
							std::cout << "Podaj ponownie drugi argument: ";
							continue;
						}
						else if (input2.length() >= 10) {
							const long long int maxVal = 2147483647;
							const long long int minVal = -2147483647;
							const long long int tempNumber = std::stoll(input2);
							if (tempNumber > maxVal || tempNumber < minVal) {
								std::cout << "Liczba wykracza poza zakres!\n";
								std::cout << "Podaj ponownie drugi argument: ";
							}
						}
						break;
					}
					const int arg1 = std::stoi(input1);
					const int arg2 = std::stoi(input2);
				}
				//Silnia
				else if (choiceCalc == 6) {
					std::string temp;
					d.ST = 's';
					int c;
					std::cout << "podaj argument  silni" << std::endl;
					std::cin >> c;
					d.SN = 1;
					if (!send_text_protocol(d, 0)) {
						std::cout << "B³¹d wysy³ania.\n";

					}
					d.number = c;
					d.SN--;
					if (!send_text_protocol(d, 2)) {
						std::cout << "B³¹d wysy³ania Silini.\n";

					}
					Sleep(100);
					receive_text_protocol(temp);
					d.from_string(temp);
					if (d.ST == "e") {
						std::cout << "error za du¿y argument \n";
						return false;
					}
					else {
						receive_text_protocol(temp);
						d.from_string(temp);
						numer_operacji++;
						std::cout << "numer operacji" << numer_operacji << "wynik wynosi: " << d.number << std::endl;
						return false;
					}
				}
			}


			//Wyœwietlanie obramowania, id sesji i tekstu odnoœnie wyboru
			CONSOLE_MANIP::clear_console();
			CONSOLE_MANIP::show_console_cursor(false);
			CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
			CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);
			CONSOLE_MANIP::print_text(2, 2, actionChoice);
		}
		return true;
	pomin_petle:
		char wybor;
		std::cout << "Wybór opcji protoko³u: \n"
			<< "- Jeœli chcesz roz³¹czyæ wyœlij 'r'\n"
			<< "- Jeœli chcesz coœ obliczyæ z 2 argumentami wyœlij 'o'\n"
			<< "- Jeœli chcesz  obliczyæ silnie wyœlij 's'\n "
			<< "- jeœli chcesz zobaczyæ historiê wyœlij 'h'\n";
		std::cout << "Wpisz swój wybór: ";
		std::cin >> wybor;
		if (wybor == 'r' || wybor == 'o' || wybor == 'h' || wybor == 's') {//
			if (wybor == 'r') {

				d.SN = 0;
				d.ST = 'r';
				if (!send_text_protocol(d, 0)) {
					std::cout << "B³¹d wysy³ania.\n";

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
					std::cout << "B³¹d wysy³ania.\n";

				}
				d.number = c;
				d.SN--;
				if (!send_text_protocol(d, 2)) {
					std::cout << "B³¹d wysy³ania Silini.\n";

				}
				Sleep(100);
				receive_text_protocol(temp);
				d.from_string(temp);
				if (d.ST == "e") {
					std::cout << "error za du¿y argument \n";
					return false;
				}
				else
				{
					receive_text_protocol(temp);
					d.from_string(temp);
					numer_operacji++;
					std::cout << "numer operacji" << numer_operacji << "wynik wynosi: " << d.number << std::endl;
					return false;
				}
			}
			if (wybor == 'o') {
				char wybor;
				bool test = true;
				std::string temp;
				d.ST = 'o';
				int c, arg2;
				do {
					std::cout << "Wybór operacji: \n"
						<< "- Jeœli chcesz dodac wyœlij 'd'\n"
						<< "- Jeœli chcesz odjac  wyœlij 'i' \n"
						<< "- Jeœli chcesz pomnozyc  wyœlij 'm'\n "
						<< "- Jeœli chcesz podzielic  wyœlij 's'\n";
					std::cout << "Wpisz swój wybór: ";
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
				d.number = c; //d.number2 = arg2;
				d.OP = wybor;// tu jest na 1 litere tego wyboru pozniej trzeba to zrobic na stringa
				std::cout << d.OP << std::endl;
				d.SN = 3;
				if (!send_text_protocol(d, 0)) {
					std::cout << "B³¹d wysy³ania.\n";

				}
				d.SN--;
				if (!send_text_protocol(d, 1)) {
					std::cout << "B³¹d wysy³ania.\n";

				}

				d.SN--;
				if (!send_text_protocol(d, 2)) {
					std::cout << "B³¹d wysy³ania Silini.\n";

				}
				d.SN--;
				if (!send_text_protocol(d, 3)) {
					std::cout << "B³¹d wysy³ania Silini.\n";

				}
				Sleep(100);
				receive_text_protocol(temp);
				d.from_string(temp);
				if (d.ST == "e") {
					std::cout << "error za du¿y argument \n";
					return false;
				}
				else
				{
					receive_text_protocol(temp);
					d.from_string(temp);
					numer_operacji++;
					std::cout << "numer operacji" << numer_operacji << "wynik dzia³ania wynosi: " << d.number << std::endl;
					return false;
				}
			}
			if (wybor == 'h') {
				int wynik;
				std::string temp;
				int num;
				std::cout << "podaj numer operacji\n";
				std::cin >> num;
				d.OP_ID = num;
				d.ST = 'h';
				d.SN = 1;
				if (!send_text_protocol(d, 0)) {
					std::cout << "B³¹d wysy³ania.\n";

				}
				d.SN--;
				if (!send_text_protocol(d, 4)) {
					std::cout << "B³¹d wysy³ania Silini.\n";

				}
				Sleep(100);
				receive_text_protocol(temp);
				d.from_string(temp);
				wynik = d.number;
				receive_text_protocol(temp);
				d.from_string(temp);
				receive_text_protocol(temp);
				d.from_string(temp);
				std::cout << "Operacja numer: " << d.OP_ID << " wynik: " << wynik << " argument1: " << d.number << " argument2: " /*<< d.number2*/ << " typ dzia³anie: " << d.OP << std::endl;

			}

		}
		else  std::cout << "wprowadziles zle dane, podaj dane ponownie" << std::endl;
	};
};


