#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Node.h"
#include <array>


//inline std::vector<std::string> GET_LOCAL_IP_ADDRESSES() {
//	char ac[80];
//	std::vector<std::string> result;
//	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
//		std::cerr << "Error " << WSAGetLastError() <<
//			" when getting local host name." << std::endl;
//		return result;
//	}
//
//	std::cout << "Host name is " << ac << "." << std::endl;
//	struct hostent *phe = gethostbyname(ac);
//	if (phe == 0) {
//		std::cerr << "Yow! Bad host lookup." << std::endl;
//		return result;
//	}
//
//	for (int i = 0; phe->h_addr_list[i] != nullptr; ++i) {
//		struct in_addr addr;
//		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
//		result.push_back(inet_ntoa(addr));
//		std::cout << "Address " << i << ": " << result[i] << std::endl;
//	}
//	result.push_back("127.0.0.1");
//	return result;
//}

class ClientUDP : public NodeUDP {
private:
	const unsigned int boxWidth = 100;
	const unsigned int boxHeight = 15;
	const std::string actionChoice = "Wybór akcji:";
	std::string sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);

	//Funkcje wprowadzania danych ----------------------------------------------------------

	//Wpisywanie 2 argumentów (u¿yte dla dodawania, odejmowania i mno¿enia)
	static void arg_input_two_add(std::array<std::string, 2>& args) {
		unsigned int argNum = 0;
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			std::cout << "Podaj równanie: " << args[0] << (argNum == 1 ? " + " + args[1] : "");
			if (argNum == 0) { CONSOLE_MANIP::input_string_int_number(args[argNum], 10); }
			else if (argNum == 1) { CONSOLE_MANIP::input_string_int_number_brackets(args[argNum], 10); }
			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				if (argNum == 1) { break; }
				argNum++;
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "                     ";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//Wpisywanie 2 argumentów (u¿yte dla dodawania, odejmowania i mno¿enia)
	static void arg_input_two_subt(std::array<std::string, 2>& args) {
		unsigned int argNum = 0;
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			std::cout << "Podaj równanie: " << args[0] << (argNum == 1 ? " - " + args[1] : "");
			if (argNum == 0) { CONSOLE_MANIP::input_string_int_number(args[argNum], 10); }
			else if (argNum == 1) { CONSOLE_MANIP::input_string_int_number_brackets(args[argNum], 10); }
			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				if (argNum == 1) { break; }
				argNum++;
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "                     ";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//Wpisywanie 2 argumentów (u¿yte dla dodawania, odejmowania i mno¿enia)
	static void arg_input_two_multp(std::array<std::string, 2>& args) {
		unsigned int argNum = 0;
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			std::cout << "Podaj równanie: " << args[0] << (argNum == 1 ? " * " + args[1] : "");
			if (argNum == 0) { CONSOLE_MANIP::input_string_int_number(args[argNum], 10); }
			else if (argNum == 1) { CONSOLE_MANIP::input_string_int_number_brackets(args[argNum], 10); }
			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				if (argNum == 1) { break; }
				argNum++;
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "                     ";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//Wpisywanie 2 argumentów dla dzielenia
	static void arg_input_two_div(std::array<std::string, 2>& args) {
		unsigned int argNum = 0;
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			std::cout << "Podaj równanie: " << args[0] << (argNum == 1 ? " / " + args[1] : "");
			if (argNum == 0) { CONSOLE_MANIP::input_string_int_number(args[argNum], 10); }
			else if (argNum == 1) { CONSOLE_MANIP::input_string_int_number_brackets(args[argNum], 10); }

			if (argNum == 1 && stod(args[argNum]) == 0) {
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 2);
				std::cout << "Dzielnik nie mo¿e byæ zerem!";
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y - 2);
				continue;
			}
			else if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				if (argNum == 1) { break; }
				argNum++;
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "                     ";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//Wpisywanie argumentu dla silni
	static void arg_input_one_uint_fact(std::array<std::string, 2>& args) {
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			std::cout << "Podaj równanie: " << args[0];
			CONSOLE_MANIP::input_string_digits(args[0], 10);

			if (stod(args[0]) < 4294967295) {
				const unsigned int currentXpos = CONSOLE_MANIP::cursor_get_pos().X;
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "                     ";
				CONSOLE_MANIP::cursor_set_pos(currentXpos, CONSOLE_MANIP::cursor_get_pos().Y - 1);
				std::cout << '!';
				break;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//Wpisywanie argumentu dla silni
	static void arg_input_one_uint(std::string& arg) {
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			std::cout << "Podaj identyfikator obliczenia: " << arg;
			CONSOLE_MANIP::input_string_digits(arg, 10);

			if (stod(arg) < 4294967295) {
				const unsigned int currentXpos = CONSOLE_MANIP::cursor_get_pos().X;
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "                     ";
				CONSOLE_MANIP::cursor_set_pos(currentXpos, CONSOLE_MANIP::cursor_get_pos().Y - 1);
				break;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				std::cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//--------------------------------------------------------------------------------------

	//Szukanie serwera i spisywanie odpoweidzi
	bool find_server() {
		//Wysy³anie ¿¹dania rozpoczêcia sesji do serwera
		for (const std::string& address : GET_IP_TABLE()) {
			//Wys³anie pola operacja
			TextProtocol operationProtocol(GET_CURRENT_TIME(), sessionId, 1);
			operationProtocol.operation = OP_BEGIN;
			send_text_protocol_to(operationProtocol, operationProtocol.get_field(), address);

			//Wys³¹nie adresu serwera
			TextProtocol addressProtocol(GET_CURRENT_TIME(), sessionId, 0);
			addressProtocol.address = address;
			send_text_protocol_to(addressProtocol, addressProtocol.get_field(), address);
		}

		//Otrzymywanie odpowiedzi na ¿¹danie rozpoczêcia sesji
		std::string clientAddress;
		bool findSuccess = false;
		while (true) {
			std::string received;
			receive_text_protocol(received);
			std::cout << "Received: " << received << '\n';
			const TextProtocol recvProtocol(received);

			if (recvProtocol.get_field() == FIELD_OPERATION && recvProtocol.operation == OP_ID_SESSION) {
				sessionId = recvProtocol.sessionId;
				std::string sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);
				findSuccess = true;
			}
			if (recvProtocol.sequenceNumber == 0) { break; }
		}

		if (!findSuccess) { return false; }

		return true;
	}

	//Wysy³anie ¿¹dania obliczenia (zale¿ne od podanej funkcji
	void calculation(void(*argInputFunc)(std::array<std::string, 2>&), const std::string& operation) {
		//Podawanie argumentów
		std::array<std::string, 2> args{ "","" };

		argInputFunc(args);

		unsigned int argNum = 0;
		for (const std::string& arg : args) { if (!arg.empty()) { argNum++; } }

		unsigned int sequenceNumber = argNum;

		//Wys³anie komunikatuu z operacj¹
		TextProtocol calcProtocol(GET_CURRENT_TIME(), sessionId, sequenceNumber);
		calcProtocol.operation = operation;
		send_text_protocol(calcProtocol, FIELD_OPERATION);
		sequenceNumber--;

		//Wys³anie argumentów
		unsigned int argsIterator = 0;
		for (const std::string& arg : args) {
			if (!arg.empty()) {
				TextProtocol numberProtocol(GET_CURRENT_TIME(), sessionId, sequenceNumber);
				numberProtocol.number = stod(args[argsIterator]);
				send_text_protocol(numberProtocol, FIELD_NUMBER);
				sequenceNumber--;
				argsIterator++;
			}
		}

		//Otrzymanie wiadomoœci zwrotnej
		std::vector<TextProtocol>receivedParts = receive_messages();

		//Parsowanie komunikatów
		for (const TextProtocol& prot : receivedParts) {
			if (prot.sessionId == sessionId) {
				if (prot.operation == OP_STATUS) {
					std::cout << " = ";
				}
				else if (prot.status == STATUS_SUCCESS) { continue; }
				else if (prot.get_field() == FIELD_NUMBER) {
					std::string numberStr = std::to_string(prot.number);
					double_remove_end_zero(numberStr);
					std::cout << (prot.number >= 0 ? numberStr : "(" + numberStr + ')');
				}
				else if (prot.status == STATUS_OUT_OF_RANGE) { std::cout << "wynik poza zakresem"; }
				else if (prot.get_field() == FIELD_CALCULATION_ID) {
					std::cout << " | Identyfikator obliczeñ: " << prot.calculationId << '\n';
				}
			}
		}
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
		system("pause");
	}

	//Wyœwietlanie sekwencji komunikatów
	void print_message_sequence(const std::vector<TextProtocol>& sequence) const {
		std::string calcSign;
		unsigned int argNum = 1;
		bool isFactorial = false;
		std::cout << '\n';
		for (const TextProtocol& prot : sequence) {
			if (prot.sessionId == sessionId) {
				//Operacje
				if (prot.get_field() == FIELD_OPERATION) {
					//Obliczenia
					if (prot.operation == OP_FACT) { calcSign = "!"; isFactorial = true; }
					else if (prot.operation == OP_ADD) { calcSign = " + "; }
					else if (prot.operation == OP_SUBT) { calcSign = " - "; }
					else if (prot.operation == OP_MULTP) { calcSign = " * "; }
					else if (prot.operation == OP_DIV) { calcSign = " / "; }
				}

				//Status
				else if (prot.get_field() == FIELD_STATUS) {
					if (prot.status == STATUS_OUT_OF_RANGE) {
						std::cout << "wynik poza zakresem";
						argNum = 1;
					}
					else if (prot.status == STATUS_FORBIDDEN) { std::cout << "Odmowa dostêpu!"; }
					else if (prot.status == STATUS_HISTORY_EMPTY) { std::cout << "Historia pusta!"; }
					else if (prot.status == STATUS_NOT_FOUND) { std::cout << "Nie znaleziono!"; }
					else if (prot.status == STATUS_SUCCESS) { std::cout << " = "; }
					else if (prot.status == STATUS_FOUND) { continue; }
				}

				//Id obliczeñ
				else if (prot.get_field() == FIELD_CALCULATION_ID) {
					std::cout << " | Identyfikator obliczenia: " << prot.calculationId << '\n';
				}

				//Argument 1
				else if (prot.get_field() == FIELD_NUMBER && argNum == 1) {
					std::string numberStr = std::to_string(prot.number);
					double_remove_end_zero(numberStr);
					std::cout << numberStr << calcSign;
					argNum++;
					if (isFactorial) { argNum++; isFactorial = false; }
				}
				//Argument 2
				else if (prot.get_field() == FIELD_NUMBER && argNum == 2) {
					std::string numberStr = std::to_string(prot.number);
					double_remove_end_zero(numberStr);
					const double numberDouble = stod(numberStr);
					std::cout << (numberDouble >= 0 ? numberStr : "(" + numberStr + ')');
					argNum++;
				}
				//Wynik
				else if (prot.get_field() == FIELD_NUMBER && argNum == 3) {
					std::string numberStr = std::to_string(prot.number);
					double_remove_end_zero(numberStr);
					const double numberDouble = stod(numberStr);
					std::cout << (numberDouble >= 0 ? numberStr : "(" + numberStr + ')');
					argNum = 1;
				}
			}
		}
		std::cout << '\n';
	}

	//Historia (dla sessionId sesji)
	void history_by_session_id() {
		TextProtocol histProtocol(GET_CURRENT_TIME(), sessionId, 0);
		histProtocol.operation = OP_HISTORY_WHOLE;
		send_text_protocol(histProtocol, FIELD_OPERATION);

		const std::vector<TextProtocol> history = receive_messages();

		print_message_sequence(history);

		system("pause");
	}

	//Historia identyfikatorze obliczeñ
	void history_by_calc_id() {
		std::string calcId;
		arg_input_one_uint(calcId);

		TextProtocol histProtocol(GET_CURRENT_TIME(), sessionId, 1);
		histProtocol.operation = OP_HISTORY_ID;
		send_text_protocol(histProtocol, FIELD_OPERATION);

		TextProtocol idProtocol(GET_CURRENT_TIME(), sessionId, 0);
		histProtocol.calculationId = std::stoi(calcId);
		send_text_protocol(histProtocol, FIELD_CALCULATION_ID);

		const std::vector<TextProtocol>history = receive_messages();

		print_message_sequence(history);

		system("pause");
	}

	//Funkcja wywo³uj¹ca wybieranie akcji i wykonuj¹ca wybór
	void action_choice() {
		sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);

		unsigned int choice = 1;
		while (true) {
			//Wyœwietlanie obramowania, sessionId sesji i tekstu odnoœnie wyboru
			CONSOLE_MANIP::clear_console();
			CONSOLE_MANIP::show_console_cursor(false);
			CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
			CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);
			CONSOLE_MANIP::print_text(2, 2, actionChoice);

			//G³ówne menu wyboru akcji
			action_choice_main_menu(choice);

			//Zakoñczenie sesji
			if (choice == 1) {
				TextProtocol protocol(GET_CURRENT_TIME(), sessionId, 0);
				protocol.operation = OP_END;
				if (!send_text_protocol(protocol, FIELD_OPERATION)) {
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

	//Menu wybierania akcji
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

	//Menu akcji obliczeñ
	void calculation_menu() {
		int choiceCalc = 1;
		std::string goBackText = " Powrót.";
		std::string additionText = " Dodanie dwóch liczb.";
		std::string subtractionText = " Odjêcie dwóch liczb.";
		std::string multiplicationText = " Mno¿enie dwóch liczb.";
		std::string divisionText = " Dzielenie dwóch liczb.";
		std::string factorialText = " Silnia z liczby.";

		while (true) {
			//Wyœwietlanie obramowania, sessionId sesji i tekstu odnoœnie wyboru
			CONSOLE_MANIP::clear_console();
			CONSOLE_MANIP::show_console_cursor(false);
			CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
			CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);
			CONSOLE_MANIP::print_text(2, 2, actionChoice);
			while (true) {
				//DODAWANIE wskaŸnika wybranej opcji
				goBackText[0] = ' ';
				additionText[0] = ' ';
				subtractionText[0] = ' ';
				multiplicationText[0] = ' ';
				divisionText[0] = ' ';
				factorialText[0] = ' ';
				if (choiceCalc == 1) { goBackText[0] = '>'; }
				else if (choiceCalc == 2) { additionText[0] = '>'; }
				else if (choiceCalc == 3) { subtractionText[0] = '>'; }
				else if (choiceCalc == 4) { multiplicationText[0] = '>'; }
				else if (choiceCalc == 5) { divisionText[0] = '>'; }
				else if (choiceCalc == 6) { factorialText[0] = '>'; }

				//Wyœwietlanie opcji
				CONSOLE_MANIP::print_text(2, 4, goBackText);
				CONSOLE_MANIP::print_text(2, 5, additionText);
				CONSOLE_MANIP::print_text(2, 6, subtractionText);
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
			if (choiceCalc == 1) { break;; }
			//Dodawanie
			else if (choiceCalc == 2) {
				calculation(&arg_input_two_add, OP_ADD);
			}
			//Odejmowanie
			else if (choiceCalc == 3) {
				calculation(&arg_input_two_subt, OP_SUBT);
			}
			//Mno¿enie
			else if (choiceCalc == 4) {
				calculation(&arg_input_two_multp, OP_MULTP);
			}
			//Dzielenie
			else if (choiceCalc == 5) {
				calculation(&arg_input_two_div, OP_DIV);
			}
			//Silnia
			else if (choiceCalc == 6) {
				calculation(&arg_input_one_uint_fact, OP_FACT);
			}
		}
	}

	//Menu historii
	void history_menu() {
		int choiceCalc = 1;
		std::string goBackText = " Powrót.";
		std::string wholeHistory = " Wyœwietl ca³¹ historiê.";
		std::string byCalcId = " Wyœwietl obliczenie o podanym identyfikatorze.";

		//Wyœwietlanie obramowania, sessionId sesji i tekstu odnoœnie wyboru
		CONSOLE_MANIP::clear_console();
		CONSOLE_MANIP::show_console_cursor(false);
		CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
		CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);
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

		//Przejœcie do wykonywania wybranej akcji

		//Powrót
		if (choiceCalc == 1) { return; }
		//Ca³a historia
		else if (choiceCalc == 2) {
			history_by_session_id();
		}
		//Po identyfikatorze obliczeñ
		else if (choiceCalc == 3) {
			history_by_calc_id();
		}
	}

public:
	unsigned int sessionId = 0;

	ClientUDP(const unsigned short& Port1) :NodeUDP(Port1) {}

	bool start_session() {
		//Szukanie serwera
		find_server();

		system("pause");

		action_choice();
		return true;
	}
};
