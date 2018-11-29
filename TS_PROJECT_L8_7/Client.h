#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"
#include <array>

class ClientUDP : public NodeUDP {
	const unsigned int boxWidth = 80;
	const unsigned int boxHeight = 20;
	const std::string actionChoice = "Wybór akcji:";
	std::string sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);
public:
	unsigned int sessionId = 0;

	ClientUDP(const u_long& IP, const unsigned short& Port1) :NodeUDP(IP, Port1) {
		otherAddr.sin_addr.S_un.S_addr = IP;
	}
	virtual ~ClientUDP() { WSACleanup(); }

	bool start_session() {
		TextProtocol startProtocol(GET_CURRENT_TIME(), sessionId, "Rozpoczecie");
		//¯¹danie rozpoczêcia sesji
		if (!send_text_protocol(startProtocol, -1)) {
			std::cout << "B³¹d wysy³ania.\n";
			system("pause");
			return false;
		}
		//Odbieranie id
		std::string received;
		if (!receive_text_protocol(received)) {
			std::cout << "B³¹d odbierania!\n";
			system("pause");
			return false;
		}
		startProtocol.from_string(received);
		if (startProtocol.operation == "IDENTYFIKATOR_SESJI") {
			sessionId = startProtocol.id;
		}
		action_choice();


		return true;
	}

	//Dodawanie
	void add() {
		//Podawanie argumentów
		std::array<std::string, 2> args;

		unsigned int argNum = 0;
		while (true) {
			std::cout << "\nPodaj argument " << argNum + 1 << " : " << args[argNum];
			CONSOLE_MANIP::input_string_int_number(args[argNum], 10);

			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				if (argNum == 1) { break; }
				argNum++;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 2);
				std::cout << "Liczba poza zakresem.";
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}

		//Wys³anie komunikatuu z operacj¹
		const TextProtocol calcProtocol(GET_CURRENT_TIME(), sessionId, "DODAWANIE");
		send_text_protocol(calcProtocol, SEND_NO_ADDITIONAL);

		//Odebranie komunikatu z id obliczeñ
		std::string received;
		receive_text_protocol(received);
		const TextProtocol calcIdProtocol(received);
		if (calcIdProtocol.operation == "IDENTYFIKATOR_OBLICZEN") {
			std::cout << "\nIdentyfikator obliczen: " << calcIdProtocol.calculationId << '\n';
		}

		//Wys³anie numeru sekwencyjnego (pocz¹tkowego)
		send_sequence_number(sessionId, 2);

		//Wys³anie argumentów
		{
			TextProtocol numberProtocol_1(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol_1.number = double(stod(args[0]));
			send_text_protocol(numberProtocol_1, SEND_NUMBER);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(sessionId, 1);

			TextProtocol numberProtocol_2(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol_2.number = double(stod(args[1]));
			send_text_protocol(numberProtocol_2, SEND_NUMBER);

		}

		//Wys³anie numeru sekwencyjnego (koñcowego)
		send_sequence_number(sessionId, 0);

		//Otrzymanie wiadomoœci zwrotnej
		std::vector<TextProtocol>receivedParts = receive_parts();

		//Parsowanie komunikatów
		for (const TextProtocol& prot : receivedParts) {
			if (prot.operation == "STATUS" && prot.status == STATUS_CALC_OUT_OF_RANGE) {
				std::cout << "\nWynik poza zakresem.\n";
				system("pause");
				break;
			}
			else if (prot.operation == "STATUS" && prot.status == STATUS_CALC_SUCCESS) {
				std::cout << "\nOperacja powiod³a siê.\n";
			}
			else if (prot.operation == "WYNIK") {
				std::cout << "\nWynik operacji: " << args[0] << " + " << args[1] << " = " << prot.number << '\n';
				system("pause");
			}
		}
	}

	//Odejmowanie
	void substract() {
		//Podawanie argumentów
		std::array<std::string, 2> args;

		unsigned int argNum = 0;
		while (true) {
			std::cout << "\nPodaj argument " << argNum + 1 << " : " << args[argNum];
			CONSOLE_MANIP::input_string_int_number(args[argNum], 10);

			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				if (argNum == 1) { break; }
				argNum++;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 2);
				std::cout << "Liczba poza zakresem.";
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}

		//Wys³anie komunikatuu z operacj¹
		const TextProtocol calcProtocol(GET_CURRENT_TIME(), sessionId, "ODEJMOWANIE");
		send_text_protocol(calcProtocol, SEND_NO_ADDITIONAL);

		//Odebranie komunikatu z id obliczeñ
		std::string received;
		receive_text_protocol(received);
		const TextProtocol calcIdProtocol(received);
		if (calcIdProtocol.operation == "IDENTYFIKATOR_OBLICZEN") {
			std::cout << "\nIdentyfikator obliczen: " << calcIdProtocol.calculationId << '\n';
		}

		//Wys³anie numeru sekwencyjnego (pocz¹tkowego)
		send_sequence_number(sessionId, 2);

		//Wys³anie argumentów
		{
			TextProtocol numberProtocol_1(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol_1.number = stod(args[0]);
			send_text_protocol(numberProtocol_1, SEND_NUMBER);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(sessionId, 1);

			TextProtocol numberProtocol_2(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol_2.number = stod(args[1]);
			send_text_protocol(numberProtocol_2, SEND_NUMBER);
		}

		//Wys³anie numeru sekwencyjnego (koñcowego)
		send_sequence_number(sessionId, 0);

		//Otrzymanie wiadomoœci zwrotnej
		std::vector<TextProtocol>receivedParts = receive_parts();

		//Parsowanie komunikatów
		for (const TextProtocol& prot : receivedParts) {
			if (prot.operation == "STATUS" && prot.status == STATUS_CALC_OUT_OF_RANGE) {
				std::cout << "\nWynik poza zakresem.\n";
				system("pause");
				break;
			}
			else if (prot.operation == "STATUS" && prot.status == STATUS_CALC_SUCCESS) {
				std::cout << "\nOperacja powiod³a siê.\n";
			}
			else if (prot.operation == "WYNIK") {
				std::cout << "\nWynik operacji: " << args[0] << " - " << args[1] << " = " << prot.number << '\n';
				system("pause");
			}
		}
	}

	//Mno¿enie
	void multiply() {
		//Podawanie argumentów
		std::array<std::string, 2> args;

		unsigned int argNum = 0;
		while (true) {
			std::cout << "\nPodaj argument " << argNum + 1 << " : " << args[argNum];
			CONSOLE_MANIP::input_string_int_number(args[argNum], 10);

			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				if (argNum == 1) { break; }
				argNum++;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 2);
				std::cout << "Liczba poza zakresem.";
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}

		//Wys³anie komunikatuu z operacj¹
		const TextProtocol calcProtocol(GET_CURRENT_TIME(), sessionId, "MNOZENIE");
		send_text_protocol(calcProtocol, SEND_NO_ADDITIONAL);

		//Odebranie komunikatu z id obliczeñ
		std::string received;
		receive_text_protocol(received);
		const TextProtocol calcIdProtocol(received);
		if (calcIdProtocol.operation == "IDENTYFIKATOR_OBLICZEN") {
			std::cout << "\nIdentyfikator obliczen: " << calcIdProtocol.calculationId << '\n';
		}

		//Wys³anie numeru sekwencyjnego (pocz¹tkowego)
		send_sequence_number(sessionId, 2);

		//Wys³anie argumentów
		{
			TextProtocol numberProtocol_1(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol_1.number = stod(args[0]);
			send_text_protocol(numberProtocol_1, SEND_NUMBER);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(sessionId, 1);

			TextProtocol numberProtocol_2(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol_2.number = double(stod(args[1]));
			send_text_protocol(numberProtocol_2, SEND_NUMBER);

		}

		//Wys³anie numeru sekwencyjnego (koñcowego)
		send_sequence_number(sessionId, 0);

		//Otrzymanie wiadomoœci zwrotnej
		std::vector<TextProtocol>receivedParts = receive_parts();

		//Parsowanie komunikatów
		for (const TextProtocol& prot : receivedParts) {
			if (prot.operation == "STATUS" && prot.status == STATUS_CALC_OUT_OF_RANGE) {
				std::cout << "\nWynik poza zakresem.\n";
				system("pause");
				break;
			}
			else if (prot.operation == "STATUS" && prot.status == STATUS_CALC_SUCCESS) {
				std::cout << "\nOperacja powiod³a siê.\n";
			}
			else if (prot.operation == "WYNIK") {
				std::cout << "\nWynik operacji: " << args[0] << " * " << args[1] << " = " << int(prot.number) << '\n';
				system("pause");
			}
		}
	}

	//Dzielenie
	void divide() {
		//Podawanie argumentów
		std::array<std::string, 2> args;
		unsigned int argNum = 0;
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 1);
			std::cout << "Podaj argument " << argNum + 1 << " : " << args[argNum];
			CONSOLE_MANIP::input_string_int_number(args[argNum], 10);

			if(argNum == 1 && stod(args[argNum]) == 0){
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 2);
				std::cout << "Dzielnik nie mo¿e byæ zerem.";
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y - 2);
				continue;
			}

			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				if (argNum == 1) { break; }
				argNum++;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "Liczba poza zakresem.";
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}

		//Wys³anie komunikatuu z operacj¹
		const TextProtocol calcProtocol(GET_CURRENT_TIME(), sessionId, "DZIELENIE");
		send_text_protocol(calcProtocol, SEND_NO_ADDITIONAL);

		//Odebranie komunikatu z id obliczeñ
		std::string received;
		receive_text_protocol(received);
		const TextProtocol calcIdProtocol(received);
		if (calcIdProtocol.operation == "IDENTYFIKATOR_OBLICZEN") {
			std::cout << "\nIdentyfikator obliczen: " << calcIdProtocol.calculationId << '\n';
		}

		//Wys³anie numeru sekwencyjnego (pocz¹tkowego)
		send_sequence_number(sessionId, 2);

		//Wys³anie argumentów
		{
			TextProtocol numberProtocol_1(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol_1.number = stod(args[0]);
			send_text_protocol(numberProtocol_1, SEND_NUMBER);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(sessionId, 1);

			TextProtocol numberProtocol_2(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol_2.number = double(stod(args[1]));
			send_text_protocol(numberProtocol_2, SEND_NUMBER);

		}

		//Wys³anie numeru sekwencyjnego (koñcowego)
		send_sequence_number(sessionId, 0);

		//Otrzymanie wiadomoœci zwrotnej
		std::vector<TextProtocol>receivedParts = receive_parts();

		//Parsowanie komunikatów
		for (const TextProtocol& prot : receivedParts) {
			if (prot.operation == "STATUS" && prot.status == STATUS_CALC_OUT_OF_RANGE) {
				std::cout << "\nWynik poza zakresem.\n";
				system("pause");
				break;
			}
			else if (prot.operation == "STATUS" && prot.status == STATUS_CALC_SUCCESS) {
				std::cout << "\nOperacja powiod³a siê.\n";
			}
			else if (prot.operation == "WYNIK") {
				std::cout << "\nWynik operacji: " << args[0] << " / " << args[1] << " = " << prot.number << '\n';
				system("pause");
			}
		}
	}

	//Silnia
	void factorial() {
		//Podawanie argumentu
		std::string arg;
		while (true) {
			std::cout << "Podaj liczbê: " << arg;
			CONSOLE_MANIP::input_string_digits(arg, 10);

			if (stod(arg) < 4294967295) {
				break;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 2);
				std::cout << "Liczba poza zakresem.";
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}

		//Wys³anie komunikatuu z operacj¹
		const TextProtocol calcProtocol(GET_CURRENT_TIME(), sessionId, "SILNIA");
		send_text_protocol(calcProtocol, SEND_NO_ADDITIONAL);

		//Odebranie komunikatu z id obliczeñ
		{
			std::string received;
			receive_text_protocol(received);
			const TextProtocol calcIdProtocol(received);
			if (calcIdProtocol.operation == "IDENTYFIKATOR_OBLICZEN") {
				std::cout << "\nIdentyfikator obliczen: " << calcIdProtocol.calculationId << '\n';
			}
		}

		//Wys³anie numeru sekwencyjnego
		send_sequence_number(sessionId, 1);

		//Wys³anie argumentu
		{
			TextProtocol numberProtocol(GET_CURRENT_TIME(), sessionId, "ARGUMENT");
			numberProtocol.number = double(stod(arg));
			send_text_protocol(numberProtocol, SEND_NUMBER);
		}

		//Wys³anie numeru sekwencyjnego
		send_sequence_number(sessionId, 0);

		//Otrzymanie wiadomoœci zwrotnej
		std::vector<TextProtocol>receivedParts = receive_parts();

		//Parsowanie komunikatów
		for (const TextProtocol& prot : receivedParts) {
			if (prot.operation == "STATUS" && prot.status == STATUS_CALC_OUT_OF_RANGE) {
				std::cout << "\nWynik poza zakresem.\n";
				system("pause");
				break;
			}
			else if (prot.operation == "STATUS" && prot.status == STATUS_CALC_SUCCESS) {
				std::cout << "\nOperacja powiod³a siê.\n";
			}
			else if (prot.operation == "WYNIK") {
				std::cout << "\nWynik operacji: " << arg << "! = " << unsigned int(prot.number) << '\n';
				system("pause");
			}
		}
	}

	//Ca³a historia
	void whole_history() {
		const TextProtocol histProtocol(GET_CURRENT_TIME(), sessionId, "HISTORIA_CALA");
		send_text_protocol(histProtocol, SEND_NO_ADDITIONAL);

		while (true) {
			std::cout << '\n';
			//Odbieranie numeru sekwencyjnego
			std::string received;
			receive_text_protocol(received);
			TextProtocol sequenceProtocol(received);

			if (sequenceProtocol.operation == "NUMER_SEKWENCYJNY" && sequenceProtocol.sequenceNumber == 0) { break; }
			else {
				//Odbieranie komunikatu od operacji
				receive_text_protocol(received);
				TextProtocol operationProtocol(received);


				//Odbieranie numeru sekwencyjnego
				receive_text_protocol(received);
				sequenceProtocol = TextProtocol(received);
				if (sequenceProtocol.operation == "NUMER_SEKWENCYJNY" && sequenceProtocol.sequenceNumber == 0) { break; }

				//Odbieranie pierwszego argumentu
				receive_text_protocol(received);
				TextProtocol argumentProtocol(received);
				std::cout << argumentProtocol.number;


				if (operationProtocol.operation == "SILNIA") {
					std::cout << "!";
				}
				else if (operationProtocol.operation == "DODAWANIE") {
					std::cout << " + ";
				}
				else if (operationProtocol.operation == "ODEJMOWANIE") {
					std::cout << " - ";
				}
				else if (operationProtocol.operation == "MNOZENIE") {
					std::cout << " * ";
				}
				else if (operationProtocol.operation == "DZIELENIE") {
					std::cout << " / ";
				}

				//Odbieranie numeru sekwencyjnego
				receive_text_protocol(received);
				sequenceProtocol = TextProtocol(received);
				if (sequenceProtocol.operation == "NUMER_SEKWENCYJNY" && sequenceProtocol.sequenceNumber == 0) { break; }

				//Odbieranie drugiego argumentu lub wyniku (silnia)
				receive_text_protocol(received);
				argumentProtocol = TextProtocol(received);

				if (operationProtocol.operation == "SILNIA") {
					std::cout << " = ";
					if (argumentProtocol.status == STATUS_CALC_OUT_OF_RANGE) {
						std::cout << argumentProtocol.status << '\n';
						continue;
					}
					else if (argumentProtocol.status == STATUS_CALC_SUCCESS) {
						//Odbieranie numeru sekwencyjnego
						receive_text_protocol(received);
						sequenceProtocol = TextProtocol(received);
						if (sequenceProtocol.operation == "NUMER_SEKWENCYJNY" && sequenceProtocol.sequenceNumber == 0) { break; }

						//Odbieranie wyniku
						receive_text_protocol(received);
						argumentProtocol = TextProtocol(received);
						std::cout << unsigned int(argumentProtocol.number);
						continue;
					}
				}
				else if (operationProtocol.operation == "ODEJMOWANIE") {
					std::cout << (-int(argumentProtocol.number) < 0 ? "(" + std::to_string(-int(argumentProtocol.number)) + ")" : std::to_string(-int(argumentProtocol.number)));
				}
				else { std::cout << int(argumentProtocol.number); }
				std::cout << " = ";

				//Odbieranie numeru sekwencyjnego
				receive_text_protocol(received);
				sequenceProtocol = TextProtocol(received);
				if (sequenceProtocol.operation == "NUMER_SEKWENCYJNY" && sequenceProtocol.sequenceNumber == 0) { break; }

				//Odbieranie wyniku
				receive_text_protocol(received);
				argumentProtocol = TextProtocol(received);

				if (argumentProtocol.status == STATUS_CALC_OUT_OF_RANGE) {
					std::cout << argumentProtocol.status << '\n';
					continue;
				}
				else if (argumentProtocol.status == STATUS_CALC_SUCCESS) {
					//Odbieranie numeru sekwencyjnego
					receive_text_protocol(received);
					sequenceProtocol = TextProtocol(received);
					if (sequenceProtocol.operation == "NUMER_SEKWENCYJNY" && sequenceProtocol.sequenceNumber == 0) { break; }

					//Odbieranie wyniku
					receive_text_protocol(received);
					argumentProtocol = TextProtocol(received);
					std::cout << argumentProtocol.number;
					continue;
				}
			}
		}
		std::cout << '\n';
		system("pause");
	}

	//Historia identyfikatorze obliczeñ
	void history_by_id() {

	}

	void action_choice_main_menu(unsigned int& choice) {
		const std::string actionChoice = "Wybór akcji:";
		std::string disconnect = " Zakoñczenie sesji.";
		std::string calculate = " Obliczenie.";
		std::string history = " Historia.";

		while (true) {
			//DODAWANIE wskaŸnika wybranej opcji
			disconnect[0] = ' ';
			calculate[0] = ' ';
			history[0] = ' ';
			if (choice == 1) {
				disconnect[0] = '>';
			}
			else if (choice == 2) {
				calculate[0] = '>';
			}
			else if (choice == 3) {
				history[0] = '>';
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
	}

	void action_choice() {
		sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);

		unsigned int choice = 1;
		while (true) {
			//Wyœwietlanie obramowania, id sesji i tekstu odnoœnie wyboru
			CONSOLE_MANIP::clear_console();
			CONSOLE_MANIP::show_console_cursor(false);
			CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
			CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);
			CONSOLE_MANIP::print_text(2, 2, actionChoice);

			//G³ówne menu wyboru akcji
			action_choice_main_menu(choice);

			//Zakoñczenie sesji
			if (choice == 1) {
				const TextProtocol protocol(GET_CURRENT_TIME(), sessionId, "ZAKONCZENIE");
				if (!send_text_protocol(protocol, SEND_NO_ADDITIONAL)) {
					std::cout << "B³¹d wysy³ania.\n";
				}
				return;
			}

			//Obliczenia
			else if (choice == 2) {
				calculation_menu();
			}

			//Historia
			else if (choice == 3) {
				history_menu();
			}
		}
	}

	void calculation_menu() {
		int choiceCalc = 1;
		std::string goBackText = " Powrót.";
		std::string additionText = " Dodanie dwóch liczb.";
		std::string substractionText = " Odjêcie dwóch liczb.";
		std::string multiplicationText = " Mno¿enie dwóch liczb.";
		std::string divisionText = " Dzielenie dwóch liczb.";
		std::string factorialText = " Silnia z liczby.";

		//Wyœwietlanie obramowania, id sesji i tekstu odnoœnie wyboru
		CONSOLE_MANIP::clear_console();
		CONSOLE_MANIP::show_console_cursor(false);
		CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
		CONSOLE_MANIP::print_text(80 - sessionIdInfo.length() - 2, 1, sessionIdInfo);
		CONSOLE_MANIP::print_text(2, 2, actionChoice);

		while (true) {
			//DODAWANIE wskaŸnika wybranej opcji
			goBackText[0] = ' ';
			additionText[0] = ' ';
			substractionText[0] = ' ';
			multiplicationText[0] = ' ';
			divisionText[0] = ' ';
			factorialText[0] = ' ';
			if (choiceCalc == 1) { goBackText[0] = '>'; }
			else if (choiceCalc == 2) { additionText[0] = '>'; }
			else if (choiceCalc == 3) { substractionText[0] = '>'; }
			else if (choiceCalc == 4) { multiplicationText[0] = '>'; }
			else if (choiceCalc == 5) { divisionText[0] = '>'; }
			else if (choiceCalc == 6) { factorialText[0] = '>'; }

			//Wyœwietlanie opcji
			CONSOLE_MANIP::print_text(2, 4, goBackText);
			CONSOLE_MANIP::print_text(2, 5, additionText);
			CONSOLE_MANIP::print_text(2, 6, substractionText);
			CONSOLE_MANIP::print_text(2, 7, multiplicationText);
			CONSOLE_MANIP::print_text(2, 8, divisionText);
			CONSOLE_MANIP::print_text(2, 9, factorialText);

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
		if (choiceCalc == 1) { return; }
		//Dodawanie
		else if (choiceCalc == 2) {
			add();
		}
		//Odejmowanie
		else if (choiceCalc == 3) {
			substract();
		}
		//Mno¿enie
		else if (choiceCalc == 4) {
			multiply();
		}
		//Dzielenie
		else if (choiceCalc == 5) {
			divide();
		}
		//Silnia
		else if (choiceCalc == 6) {
			factorial();
		}
	}

	void history_menu() {
		int choiceCalc = 1;
		std::string goBackText = " Powrót.";
		std::string wholeHistory = " Wyœwietl ca³¹ historiê.";
		std::string byCalcId = " Wyœwietl równanie o podanym identyfikatorze.";

		//Wyœwietlanie obramowania, id sesji i tekstu odnoœnie wyboru
		CONSOLE_MANIP::clear_console();
		CONSOLE_MANIP::show_console_cursor(false);
		CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
		CONSOLE_MANIP::print_text(80 - sessionIdInfo.length() - 2, 1, sessionIdInfo);
		CONSOLE_MANIP::print_text(2, 2, actionChoice);

		while (true) {
			//DODAWANIE wskaŸnika wybranej opcji
			goBackText[0] = ' ';
			wholeHistory[0] = ' ';
			byCalcId[0] = ' ';
			if (choiceCalc == 1) { goBackText[0] = '>'; }
			else if (choiceCalc == 2) { wholeHistory[0] = '>'; }
			else if (choiceCalc == 3) { byCalcId[0] = '>'; }

			//Wyœwietlanie opcji
			CONSOLE_MANIP::print_text(2, 4, goBackText);
			CONSOLE_MANIP::print_text(2, 5, wholeHistory);
			CONSOLE_MANIP::print_text(2, 6, byCalcId);

			//Czyszczynie bufora wejœcia, aby po wduszeniu przycisku,
			// jego akcja nie zosta³a wielokrotnie wykonana
			CONSOLE_MANIP::clear_console_input_buffer();

			//Sprawdzanie naciœniêtych klawiszy
			if (CONSOLE_MANIP::check_arrow("UP") && choiceCalc > 1) { choiceCalc--; }
			else if (CONSOLE_MANIP::check_arrow("DOWN") && choiceCalc < 3) { choiceCalc++; }
			else if (CONSOLE_MANIP::check_enter()) { break; }
		}
		//Przejœcie do wykonywania wybranego dzia³ania
		//Powrót
		if (choiceCalc == 1) { return; }
		//Ca³a historia
		else if (choiceCalc == 2) {
			whole_history();
		}
		//Po identyfikatorze obliczeñ
		else if (choiceCalc == 3) {
			history_by_id();
		}
	}
};
