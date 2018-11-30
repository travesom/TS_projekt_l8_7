#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"
#include <array>

class ClientUDP : public NodeUDP {
	const unsigned int boxWidth = 80;
	const unsigned int boxHeight = 20;
	const std::string actionChoice = "Wyb�r akcji:";
	std::string sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);
public:
	unsigned int sessionId = 0;

	ClientUDP(const u_long& IP, const unsigned short& Port1) :NodeUDP(IP, Port1) {}

	bool start_session() {
		TextProtocol startProtocol(GET_CURRENT_TIME(), sessionId, 0);
		startProtocol.operation = OP_BEGIN;
		//��danie rozpocz�cia sesji
		if (!send_text_protocol(startProtocol, FIELD_OPERATION)) {
			std::cout << "B��d wysy�ania.\n";
			system("pause");
			return false;
		}
		//Odbieranie id
		std::string received;
		if (!receive_text_protocol(received)) {
			std::cout << "B��d odbierania!\n";
			system("pause");
			return false;
		}
		startProtocol.from_string(received);
		if (startProtocol.operation == OP_ID_SESSION) {
			sessionId = startProtocol.id;
		}

		action_choice();

		return true;
	}

	static void arg_input_two(std::array<std::string, 2>& args) {
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
	}

	static void arg_input_one_uint(std::array<std::string, 2>& args) {
		while (true) {
			std::cout << "Podaj liczb�: " << args[0];
			CONSOLE_MANIP::input_string_digits(args[0], 10);

			if (stod(args[0]) < 4294967295) {
				break;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 2);
				std::cout << "Liczba poza zakresem.";
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	static void arg_input_two_div(std::array<std::string, 2>& args) {
		unsigned int argNum = 0;
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 1);
			std::cout << "Podaj argument " << argNum + 1 << " : " << args[argNum];
			CONSOLE_MANIP::input_string_int_number(args[argNum], 10);

			if (argNum == 1 && stod(args[argNum]) == 0) {
				CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y + 2);
				std::cout << "Dzielnik nie mo�e by� zerem.";
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
	}

	//Wysy�anie ��dania obliczenia (zale�ne od podanej funkcji
	void calculation(void(*argInputFunc)(std::array<std::string, 2>&), const std::string& operation) {
		//Podawanie argument�w
		std::array<std::string, 2> args{ "","" };

		argInputFunc(args);

		unsigned int argNum = 0;
		for (const std::string& arg : args) { if (!arg.empty()) { argNum++; } }

		unsigned int sequenceNumber = argNum;

		//Wys�anie komunikatuu z operacj�
		TextProtocol calcProtocol(GET_CURRENT_TIME(), sessionId, sequenceNumber);
		calcProtocol.operation = operation;
		send_text_protocol(calcProtocol, FIELD_OPERATION);
		sequenceNumber--;

		//Wys�anie argument�w
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


		//Otrzymanie wiadomo�ci zwrotnej
		std::vector<TextProtocol>receivedParts = receive_parts();

		//Parsowanie komunikat�w
		for (const TextProtocol& prot : receivedParts) {
			if (prot.operation == OP_RESULT) {
				if (operation == OP_FACT) { std::cout << "\nWynik operacji: " << args[0] << "! = "; }
				else {
					std::string calcSign;
					if (operation == OP_ADD) { calcSign = " + "; }
					else if (operation == OP_SUBT) { calcSign = " - "; }
					else if (operation == OP_MULTP) { calcSign = " * "; }
					else if (operation == OP_DIV) { calcSign = " / "; }
					std::cout << "\nWynik operacji: " << args[0] << calcSign << (stod(args[1]) >= 0.0 ? args[1] : "(" + args[1] + ')') << " = ";
				}
			} //Dla tego komunikatu nic nie robimy
			else if (prot.status == STATUS_SUCCESS) {
				continue;
			}
			else if (prot.get_field() == FIELD_NUMBER) {
				std::string numberStr = std::to_string(prot.number);
				double_remove_end_zero(numberStr);
				std::cout << (prot.number >= 0 ? numberStr : "(" + numberStr + ')');
			}
			else if (prot.status == STATUS_OUT_OF_RANGE) { std::cout << "wynik poza zakresem"; }
			else if (prot.get_field() == FIELD_CALCULATION_ID) {
				std::cout << " | Identyfikator oblicze�: " << prot.calculationId << '\n';
			}
		}
		system("pause");
	}

	//Ca�a historia
	void whole_history() {
		TextProtocol histProtocol(GET_CURRENT_TIME(), sessionId, 0);
		histProtocol.operation = OP_HISTORY_WHOLE;
		send_text_protocol(histProtocol, FIELD_OPERATION);

		std::vector<TextProtocol> history = receive_parts();

		std::string calcSign;
		unsigned int argNum = 1;
		bool isFactorial = false;
		std::cout << '\n';
		for (const TextProtocol& prot : history) {
			if (prot.operation == OP_FACT) { calcSign = "!"; isFactorial = true; }
			else if (prot.operation == OP_ADD) { calcSign = " + "; }
			else if (prot.operation == OP_SUBT) { calcSign = " - "; }
			else if (prot.operation == OP_MULTP) { calcSign = " * "; }
			else if (prot.operation == OP_DIV) { calcSign = " / "; }

			else if (prot.operation == OP_RESULT) { std::cout << " = "; }
			else if (prot.status == STATUS_OUT_OF_RANGE) {
				std::cout << prot.status;
				argNum = 1;
			}
			else if (prot.status == STATUS_SUCCESS) { continue; }
			else if (prot.get_field() == FIELD_CALCULATION_ID) {
				std::cout << " | Identyfikator obliczenia: " << prot.calculationId << '\n';
			}

			//Argument 1
			else if (!isnan(prot.number) && argNum == 1) {
				std::string numberStr = std::to_string(prot.number);
				double_remove_end_zero(numberStr);
				std::cout << numberStr << calcSign;
				argNum++;
				if (isFactorial) { argNum++; isFactorial = false; }
			}
			//Argument 2
			else if (!isnan(prot.number) && argNum == 2) {
				std::string numberStr = std::to_string(prot.number);
				double_remove_end_zero(numberStr);
				const double numberDouble = stod(numberStr);
				std::cout << (numberDouble >= 0 ? numberStr : "(" + numberStr + ')');
				argNum++;
			}
			//Wynik
			else if (!isnan(prot.number) && argNum == 3) {
				std::string numberStr = std::to_string(prot.number);
				double_remove_end_zero(numberStr);
				const double numberDouble = stod(numberStr);
				std::cout << (numberDouble >= 0 ? numberStr : "(" + numberStr + ')');
				argNum = 1;
			}
		}
		std::cout << '\n';
		system("pause");
	}

	//Historia identyfikatorze oblicze�
	void history_by_id() {

	}

	void action_choice_main_menu(unsigned int& choice) {
		const std::string actionChoice = "Wyb�r akcji:";
		std::string disconnect = " Zako�czenie sesji.";
		std::string calculate = " Obliczenie.";
		std::string history = " Historia.";

		while (true) {
			//DODAWANIE wska�nika wybranej opcji
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

			//Wy�wietlanie opcji
			CONSOLE_MANIP::print_text(2, 4, disconnect);
			CONSOLE_MANIP::print_text(2, 5, calculate);
			CONSOLE_MANIP::print_text(2, 6, history);

			//Czyszczynie bufora wej�cia, aby po wduszeniu przycisku,
			// jego akcja nie zosta�a wielokrotnie wykonana
			CONSOLE_MANIP::clear_console_input_buffer();

			//Sprawdzanie naci�ni�tych klawiszy
			if (CONSOLE_MANIP::check_arrow("UP") && choice > 1) { choice--; }
			else if (CONSOLE_MANIP::check_arrow("DOWN") && choice < 3) { choice++; }
			else if (CONSOLE_MANIP::check_enter()) { break; }
		}
	}

	void action_choice() {
		sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);

		unsigned int choice = 1;
		while (true) {
			//Wy�wietlanie obramowania, id sesji i tekstu odno�nie wyboru
			CONSOLE_MANIP::clear_console();
			CONSOLE_MANIP::show_console_cursor(false);
			CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
			CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);
			CONSOLE_MANIP::print_text(2, 2, actionChoice);

			//G��wne menu wyboru akcji
			action_choice_main_menu(choice);

			//Zako�czenie sesji
			if (choice == 1) {
				TextProtocol protocol(GET_CURRENT_TIME(), sessionId, 0);
				protocol.operation = OP_END;
				if (!send_text_protocol(protocol, FIELD_OPERATION)) {
					std::cout << "B��d wysy�ania.\n";
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
		std::string goBackText = " Powr�t.";
		std::string additionText = " Dodanie dw�ch liczb.";
		std::string subtractionText = " Odj�cie dw�ch liczb.";
		std::string multiplicationText = " Mno�enie dw�ch liczb.";
		std::string divisionText = " Dzielenie dw�ch liczb.";
		std::string factorialText = " Silnia z liczby.";

		while (true) {
			//Wy�wietlanie obramowania, id sesji i tekstu odno�nie wyboru
			CONSOLE_MANIP::clear_console();
			CONSOLE_MANIP::show_console_cursor(false);
			CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
			CONSOLE_MANIP::print_text(80 - sessionIdInfo.length() - 2, 1, sessionIdInfo);
			CONSOLE_MANIP::print_text(2, 2, actionChoice);
			while (true) {
				//DODAWANIE wska�nika wybranej opcji
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

				//Wy�wietlanie opcji
				CONSOLE_MANIP::print_text(2, 4, goBackText);
				CONSOLE_MANIP::print_text(2, 5, additionText);
				CONSOLE_MANIP::print_text(2, 6, subtractionText);
				CONSOLE_MANIP::print_text(2, 7, multiplicationText);
				CONSOLE_MANIP::print_text(2, 8, divisionText);
				CONSOLE_MANIP::print_text(2, 9, factorialText);

				//Czyszczynie bufora wej�cia, aby po wduszeniu przycisku,
				// jego akcja nie zosta�a wielokrotnie wykonana
				CONSOLE_MANIP::clear_console_input_buffer();

				//Sprawdzanie naci�ni�tych klawiszy
				if (CONSOLE_MANIP::check_arrow("UP") && choiceCalc > 1) { choiceCalc--; }
				else if (CONSOLE_MANIP::check_arrow("DOWN") && choiceCalc < 6) { choiceCalc++; }
				else if (CONSOLE_MANIP::check_enter()) { break; }
			}
			//Przej�cie do wykonywania wybranego dzia�ania
			//Powr�t
			if (choiceCalc == 1) { break;; }
			//Dodawanie
			else if (choiceCalc == 2) {
				calculation(&arg_input_two, OP_ADD);
			}
			//Odejmowanie
			else if (choiceCalc == 3) {
				calculation(&arg_input_two, OP_SUBT);
			}
			//Mno�enie
			else if (choiceCalc == 4) {
				calculation(&arg_input_two, OP_MULTP);
			}
			//Dzielenie
			else if (choiceCalc == 5) {
				calculation(&arg_input_two_div, OP_DIV);
			}
			//Silnia
			else if (choiceCalc == 6) {
				calculation(&arg_input_one_uint, OP_FACT);
			}
		}
	}

	void history_menu() {
		int choiceCalc = 1;
		std::string goBackText = " Powr�t.";
		std::string wholeHistory = " Wy�wietl ca�� histori�.";
		std::string byCalcId = " Wy�wietl r�wnanie o podanym identyfikatorze.";

		//Wy�wietlanie obramowania, id sesji i tekstu odno�nie wyboru
		CONSOLE_MANIP::clear_console();
		CONSOLE_MANIP::show_console_cursor(false);
		CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
		CONSOLE_MANIP::print_text(80 - sessionIdInfo.length() - 2, 1, sessionIdInfo);
		CONSOLE_MANIP::print_text(2, 2, actionChoice);

		while (true) {
			//DODAWANIE wska�nika wybranej opcji
			goBackText[0] = ' ';
			wholeHistory[0] = ' ';
			byCalcId[0] = ' ';
			if (choiceCalc == 1) { goBackText[0] = '>'; }
			else if (choiceCalc == 2) { wholeHistory[0] = '>'; }
			else if (choiceCalc == 3) { byCalcId[0] = '>'; }

			//Wy�wietlanie opcji
			CONSOLE_MANIP::print_text(2, 4, goBackText);
			CONSOLE_MANIP::print_text(2, 5, wholeHistory);
			CONSOLE_MANIP::print_text(2, 6, byCalcId);

			//Czyszczynie bufora wej�cia, aby po wduszeniu przycisku,
			// jego akcja nie zosta�a wielokrotnie wykonana
			CONSOLE_MANIP::clear_console_input_buffer();

			//Sprawdzanie naci�ni�tych klawiszy
			if (CONSOLE_MANIP::check_arrow("UP") && choiceCalc > 1) { choiceCalc--; }
			else if (CONSOLE_MANIP::check_arrow("DOWN") && choiceCalc < 3) { choiceCalc++; }
			else if (CONSOLE_MANIP::check_enter()) { break; }
		}
		//Przej�cie do wykonywania wybranego dzia�ania
		//Powr�t
		if (choiceCalc == 1) { return; }
		//Ca�a historia
		else if (choiceCalc == 2) {
			whole_history();
		}
		//Po identyfikatorze oblicze�
		else if (choiceCalc == 3) {
			history_by_id();
		}
	}
};
