#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Node.h"
#include <array>

class ClientUDP : public NodeUDP {
private:
	const unsigned int boxWidth = 100;
	const unsigned int boxHeight = 14;
	const std::string actionChoice = "Wybór akcji:";
	std::string sessionIdInfo;



public:
	unsigned int sessionId = 0;

	//Konstruktor
	explicit ClientUDP(const unsigned short& Port1) :NodeUDP(Port1) {
		messages = false;
		set_receive_timeout(recvTimeout);
	}

	//Destruktor
	~ClientUDP() {}

	//Funkcja rozpoczynaj¹ca sesjê
	bool start_session() {
		//Szukanie serwera
		if (!find_server()) {
			if (otherAddr.sin_addr.s_addr == inet_addr("127.0.0.1")) { otherAddr.sin_port -= 1; }
			return false;
		}

		action_choice();

		if (otherAddr.sin_addr.s_addr == inet_addr("127.0.0.1")) { otherAddr.sin_port -= 1; }
		return true;
	}


private:
	//Wyœwietlanie napisu "Poszukiwanie serwera
	static void find_server_text(bool& stop) {
		auto timeStart = std::chrono::system_clock::now();
		const std::string searchingText = "Szukanie serwera ";
		unsigned int dotNumber = 0;
		std::string dotsText;

		CONSOLE_MANIP::print_text(3, 2, searchingText + dotsText);
		while (true) {
			std::chrono::duration<double> time = std::chrono::system_clock::now() - timeStart;
			if (time >= std::chrono::duration<double>(0.5)) {
				dotNumber++;
				if (dotNumber > 3) {
					dotNumber = 0;
					dotsText.clear();
				}
				else { dotsText += ". "; }
				CONSOLE_MANIP::print_text(3, 2, searchingText + "                  ");
				CONSOLE_MANIP::print_text(3, 2, searchingText + dotsText);
				timeStart = std::chrono::system_clock::now();
			}
			if (stop) {
				return;
			}
		}
	}

	//Szukanie serwera i spisywanie odpoweidzi
	bool find_server() {
		CONSOLE_MANIP::clear_console();
		CONSOLE_MANIP::show_console_cursor(false);

		//W³¹czenie w¹tku do wyœwietlania animacji szuaknie serwera
		bool textStop = false;
		std::thread textThread(&ClientUDP::find_server_text, std::ref(textStop));

		//Wysy³anie ¿¹dania rozpoczêcia sesji do serwera
		const auto send_op_begin = [this]() {
			for (const std::string& address : GET_IP_TABLE()) {
				//Wys³anie pola operacja
				TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 2);
				send_text_protocol_to(statusProtocol, FIELD_STATUS, address);

				//Wys³anie pola operacja
				TextProtocol operationProtocol(GET_CURRENT_TIME(), sessionId, 1);
				operationProtocol.operation = OP_BEGIN;
				send_text_protocol_to(operationProtocol, operationProtocol.get_field(), address);

				//Wys³anie adresu serwera
				TextProtocol addressProtocol(GET_CURRENT_TIME(), sessionId, 0);
				addressProtocol.address = address;
				send_text_protocol_to(addressProtocol, addressProtocol.get_field(), address);
			}
		};

		//Wykonanie powy¿szego wysy³ania
		send_op_begin();

		//Otrzymywanie odpowiedzi na ¿¹danie rozpoczêcia sesji
		bool findSuccess = false;
		byte failCount = 0;
		while (true) {
			std::string received;
			if (receive_text_protocol(received)) {
				const TextProtocol recvProtocol(received);
				if (recvProtocol.operation == OP_ID_SESSION) {
					sessionId = recvProtocol.sessionId;
					sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);
					findSuccess = true;
				}
				if (recvProtocol.sequenceNumber == 0) { break; }
			}
			else {
				CONSOLE_MANIP::print_text(3 + 2 * failCount, 1, "X");
				failCount++;
			}
			if (failCount >= 11) { break; }
			else {
				//Wysy³anie ¿¹dania rozpoczêcia sesji do serwera
				send_op_begin();
			}
		}

		if (findSuccess) {
			//Wys³anie pola operacja
			TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 1);
			send_text_protocol(statusProtocol, FIELD_STATUS);

			//Wys³anie potwierdzenia
			TextProtocol ackProtocol(GET_CURRENT_TIME(), sessionId, 0);
			ackProtocol.operation = OP_ACK;
			Sleep(100);
			send_text_protocol(ackProtocol, ackProtocol.get_field());
		}

		//Zatrzymanie w¹tku do wyœwietlania animacji szukanie serwera
		textStop = true;
		textThread.join();
		return findSuccess;
	}



	//Funkcje wprowadzania danych ----------------------------------------------------------

	//Wpisywanie 2 argumentów (u¿yte dla dodawania, odejmowania i mno¿enia)
	static void arg_input_two_add(std::array<std::string, 2>& args) {
		unsigned int argNum = 0;
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			sync_cout << "Podaj równanie: " << args[0] << (argNum == 1 ? " + " + args[1] : "");
			if (argNum == 0) { CONSOLE_MANIP::input_string_int_number(args[argNum], 10); }
			else if (argNum == 1) { CONSOLE_MANIP::input_string_int_number_brackets(args[argNum], 10); }
			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				const COORD cursorPos = CONSOLE_MANIP::cursor_get_pos();
				CONSOLE_MANIP::print_text(2, CONSOLE_MANIP::cursor_get_pos().Y + 1, "                     ");
				CONSOLE_MANIP::cursor_set_pos(cursorPos);
				if (argNum == 1) { break; }
				argNum++;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				sync_cout << "Liczba poza zakresem!";
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
			sync_cout << "Podaj równanie: " << args[0] << (argNum == 1 ? " - " + args[1] : "");
			if (argNum == 0) { CONSOLE_MANIP::input_string_int_number(args[argNum], 10); }
			else if (argNum == 1) { CONSOLE_MANIP::input_string_int_number_brackets(args[argNum], 10); }
			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				const COORD cursorPos = CONSOLE_MANIP::cursor_get_pos();
				CONSOLE_MANIP::print_text(2, CONSOLE_MANIP::cursor_get_pos().Y + 1, "                     ");
				CONSOLE_MANIP::cursor_set_pos(cursorPos);
				if (argNum == 1) { break; }
				argNum++;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				sync_cout << "Liczba poza zakresem!";
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
			sync_cout << "Podaj równanie: " << args[0] << (argNum == 1 ? " * " + args[1] : "");
			if (argNum == 0) { CONSOLE_MANIP::input_string_int_number(args[argNum], 10); }
			else if (argNum == 1) { CONSOLE_MANIP::input_string_int_number_brackets(args[argNum], 10); }
			if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				const COORD cursorPos = CONSOLE_MANIP::cursor_get_pos();
				CONSOLE_MANIP::print_text(2, CONSOLE_MANIP::cursor_get_pos().Y + 1, "                     ");
				CONSOLE_MANIP::cursor_set_pos(cursorPos);
				if (argNum == 1) { break; }
				argNum++;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				sync_cout << "Liczba poza zakresem!";
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
			sync_cout << "Podaj równanie: " << args[0] << (argNum == 1 ? " / " + args[1] : "");
			if (argNum == 0) { CONSOLE_MANIP::input_string_int_number(args[argNum], 10); }
			else if (argNum == 1) { CONSOLE_MANIP::input_string_int_number_brackets(args[argNum], 10); }

			if (argNum == 1 && stod(args[argNum]) == 0) {
				const COORD cursorPos = CONSOLE_MANIP::cursor_get_pos();
				CONSOLE_MANIP::print_text(2, CONSOLE_MANIP::cursor_get_pos().Y + 1, "Dzielnik nie mo¿e byæ zerem!");
				CONSOLE_MANIP::cursor_set_pos(cursorPos);
				continue;
			}
			else if (stod(args[argNum]) < 2147483647 && stod(args[argNum]) > -2147483647) {
				const COORD cursorPos = CONSOLE_MANIP::cursor_get_pos();
				CONSOLE_MANIP::print_text(2, CONSOLE_MANIP::cursor_get_pos().Y + 1, "                              ");
				CONSOLE_MANIP::cursor_set_pos(cursorPos);
				if (argNum == 1) { break; }
				argNum++;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				sync_cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//Wpisywanie argumentu dla silni
	static void arg_input_one_uint_fact(std::array<std::string, 2>& args) {
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			sync_cout << "Podaj równanie: " << args[0];
			CONSOLE_MANIP::input_string_digits(args[0], 10);

			if (stod(args[0]) < 4294967295) {
				const unsigned int currentXpos = CONSOLE_MANIP::cursor_get_pos().X;
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				sync_cout << "                     ";
				CONSOLE_MANIP::cursor_set_pos(currentXpos, CONSOLE_MANIP::cursor_get_pos().Y - 1);
				sync_cout << '!';
				break;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				sync_cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//Wpisywanie argumentu dla silni
	static void arg_input_one_uint(std::string& arg) {
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 2);
		while (true) {
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y);
			sync_cout << "Podaj identyfikator obliczenia: " << arg;
			CONSOLE_MANIP::input_string_digits(arg, 10);

			if (stod(arg) < 4294967295) {
				const unsigned int currentXpos = CONSOLE_MANIP::cursor_get_pos().X;
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				sync_cout << "                     ";
				CONSOLE_MANIP::cursor_set_pos(currentXpos, CONSOLE_MANIP::cursor_get_pos().Y - 1);
				break;
			}
			else {
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
				sync_cout << "Liczba poza zakresem!";
				CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y - 1);
			}
		}
	}

	//--------------------------------------------------------------------------------------



	//Wysy³anie ¿¹dania obliczenia (zale¿ne od podanej funkcji)
	void calculation(void(*argInputFunc)(std::array<std::string, 2>&), const std::string& operation) {
		//Podawanie argumentów
		std::array<std::string, 2> args{ "","" };

		argInputFunc(args);

		unsigned int argNum = 0;
		for (const std::string& arg : args) { if (!arg.empty()) { argNum++; } }

		unsigned int sequenceNumber = argNum + 1;

		//Wys³anie pola operacja
		TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, sequenceNumber);
		send_text_protocol(statusProtocol, FIELD_STATUS);
		sequenceNumber--;

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
		std::vector<TextProtocol>receivedMessages;
		while (true) {
			std::string received;
			if (receive_text_protocol(received)) {
				const TextProtocol receivedProtocol(received);
				if (receivedProtocol.operation != OP_ID_SESSION && receivedProtocol.status != STATUS_NOT_FOUND &&
					receivedProtocol.status != STATUS_FORBIDDEN && receivedProtocol.status != STATUS_FOUND &&
					receivedProtocol.status != STATUS_HISTORY_EMPTY) {
					receivedMessages.push_back(receivedProtocol);
					//Odbieranie koñczy siê przy natrafieniu na numer sekwencyjny 0
					if (receivedProtocol.sequenceNumber == 0 && receivedProtocol.sessionId != 0) { break; }
				}
			}
		}

		//Parsowanie komunikatów
		for (const TextProtocol& prot : receivedMessages) {
			if (prot.sessionId == sessionId) {
				if (prot.operation == OP_STATUS) {
					sync_cout << " = ";
				}
				else if (prot.status == STATUS_SUCCESS) { continue; }
				else if (prot.get_field() == FIELD_NUMBER) {
					std::string numberStr = std::to_string(prot.number);
					double_remove_end_zero(numberStr);
					sync_cout << (prot.number >= 0 ? numberStr : "(" + numberStr + ')');
				}
				else if (prot.status == STATUS_OUT_OF_RANGE) { sync_cout << "wynik poza zakresem"; }
				else if (prot.get_field() == FIELD_CALCULATION_ID) {
					sync_cout << " | Identyfikator obliczeñ: " << prot.calculationId << '\n';
				}
			}
		}
		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
		CONSOLE_MANIP::press_any_key_pause();
	}

	//Wyœwietlanie sekwencji komunikatów
	void print_message_sequence_hist_whole(const std::vector<TextProtocol>& sequence) const {
		std::string calcSign;
		unsigned int argNum = 1;
		bool isFactorial = false;

		//Wyœwietlanie obramowania, sessionId sesji i tekstu odnoœnie wyboru
		if (!sequence.empty()) {
			if (sequence[0].sequenceNumber != 0) {
				CONSOLE_MANIP::clear_console();
				CONSOLE_MANIP::show_console_cursor(false);
			}

			unsigned int equationNumber = 0;
			CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 3);

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
							sync_cout << " = wynik poza zakresem";
							argNum = 1;
						}
						else if (prot.status == STATUS_FORBIDDEN) {
							CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
							sync_cout << "Odmowa dostêpu!";
							CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
						}
						else if (prot.status == STATUS_HISTORY_EMPTY) {
							CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
							sync_cout << "Historia pusta!";
							CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
						}
						else if (prot.status == STATUS_NOT_FOUND) {
							CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
							sync_cout << "Nie znaleziono!";
							CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
						}
						else if (prot.status == STATUS_SUCCESS) { sync_cout << " = "; }
						else if (prot.status == STATUS_FOUND) { continue; }
					}

					//Id obliczeñ
					else if (prot.get_field() == FIELD_CALCULATION_ID) {
						CONSOLE_MANIP::cursor_set_pos(boxWidth - 40, CONSOLE_MANIP::cursor_get_pos().Y);
						CONSOLE_MANIP::print_text(2, CONSOLE_MANIP::cursor_get_pos().Y, "Identyfikator obliczenia: " + std::to_string(prot.calculationId) + "\n");
						equationNumber++;
					}

					//Argument 1
					else if (prot.get_field() == FIELD_NUMBER && argNum == 1) {
						std::string numberStr = std::to_string(prot.number);
						double_remove_end_zero(numberStr);

						CONSOLE_MANIP::print_text(40, CONSOLE_MANIP::cursor_get_pos().Y, "| " + numberStr + calcSign);
						argNum++;
						if (isFactorial) { argNum++; isFactorial = false; }
					}
					//Argument 2
					else if (prot.get_field() == FIELD_NUMBER && argNum == 2) {
						std::string numberStr = std::to_string(prot.number);
						double_remove_end_zero(numberStr);
						const double numberDouble = stod(numberStr);
						sync_cout << (numberDouble >= 0 ? numberStr : "(" + numberStr + ')');
						argNum++;
					}
					//Wynik
					else if (prot.get_field() == FIELD_NUMBER && argNum == 3) {
						std::string numberStr = std::to_string(prot.number);
						double_remove_end_zero(numberStr);
						const double numberDouble = stod(numberStr);
						sync_cout << (numberDouble >= 0 ? numberStr : "(" + numberStr + ')');
						argNum = 1;
					}
				}
			}

			if (sequence[0].sequenceNumber != 0) {
				const COORD cursorPos = CONSOLE_MANIP::cursor_get_pos();
				CONSOLE_MANIP::print_text(2, 1, "Ca³a Twoja Historia");
				CONSOLE_MANIP::print_box(0, 0, boxWidth, equationNumber + 4);
				CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);
				CONSOLE_MANIP::cursor_set_pos(cursorPos);
			}
		}
	}

	//Historia (dla identyfikatora sesji)
	void history_by_session_id() {
		//Wys³anie pola operacja
		TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 1);
		send_text_protocol(statusProtocol, FIELD_STATUS);

		TextProtocol histProtocol(GET_CURRENT_TIME(), sessionId, 0);
		histProtocol.operation = OP_HISTORY_WHOLE;
		send_text_protocol(histProtocol, FIELD_OPERATION);

		std::vector<TextProtocol> history;
		while (true) {
			std::string received;
			if (receive_text_protocol(received)) {
				const TextProtocol receivedProtocol(received);
				if (receivedProtocol.operation != OP_ID_SESSION && receivedProtocol.status != STATUS_NOT_FOUND &&
					receivedProtocol.status != STATUS_FORBIDDEN && receivedProtocol.status != STATUS_FOUND) {
					history.push_back(receivedProtocol);
					//Odbieranie koñczy siê przy natrafieniu na numer sekwencyjny 0
					if (receivedProtocol.sequenceNumber == 0 && receivedProtocol.sessionId != 0) { break; }
				}
			}
		}


		print_message_sequence_hist_whole(history);

		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
		CONSOLE_MANIP::press_any_key_pause();
	}

	//Wyœwietlanie sekwencji komunikatów
	void print_message_sequence(const std::vector<TextProtocol>& sequence) const {
		std::string calcSign;
		unsigned int argNum = 1;
		bool isFactorial = false;

		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
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
						sync_cout << " = wynik poza zakresem";
						argNum = 1;
					}
					else if (prot.status == STATUS_FORBIDDEN) {
						CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
						sync_cout << "Odmowa dostêpu!";
						CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
					}
					else if (prot.status == STATUS_HISTORY_EMPTY) {
						CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
						sync_cout << "Historia pusta!";
						CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
					}
					else if (prot.status == STATUS_NOT_FOUND) {
						CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
						sync_cout << "Nie znaleziono!";
						CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
					}
					else if (prot.status == STATUS_SUCCESS) { sync_cout << " = "; }
					else if (prot.status == STATUS_FOUND) { continue; }
				}

				//Id obliczeñ
				else if (prot.get_field() == FIELD_CALCULATION_ID) {
					sync_cout << " | Identyfikator obliczenia: " << prot.calculationId << '\n';
				}

				//Argument 1
				else if (prot.get_field() == FIELD_NUMBER && argNum == 1) {
					std::string numberStr = std::to_string(prot.number);
					double_remove_end_zero(numberStr);

					CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
					sync_cout << numberStr << calcSign;
					argNum++;
					if (isFactorial) { argNum++; isFactorial = false; }
				}
				//Argument 2
				else if (prot.get_field() == FIELD_NUMBER && argNum == 2) {
					std::string numberStr = std::to_string(prot.number);
					double_remove_end_zero(numberStr);
					const double numberDouble = stod(numberStr);
					sync_cout << (numberDouble >= 0 ? numberStr : "(" + numberStr + ')');
					argNum++;
				}
				//Wynik
				else if (prot.get_field() == FIELD_NUMBER && argNum == 3) {
					std::string numberStr = std::to_string(prot.number);
					double_remove_end_zero(numberStr);
					const double numberDouble = stod(numberStr);
					sync_cout << (numberDouble >= 0 ? numberStr : "(" + numberStr + ')');
					argNum = 1;
				}
			}
		}
	}

	//Historia identyfikatorze obliczeñ
	void history_by_calc_id() {
		std::string calcId;
		arg_input_one_uint(calcId);

		//Wys³anie pola operacja
		TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 2);
		send_text_protocol(statusProtocol, FIELD_STATUS);

		TextProtocol histProtocol(GET_CURRENT_TIME(), sessionId, 1);
		histProtocol.operation = OP_HISTORY_ID;
		send_text_protocol(histProtocol, FIELD_OPERATION);

		TextProtocol idProtocol(GET_CURRENT_TIME(), sessionId, 0);
		histProtocol.calculationId = std::stoi(calcId);
		send_text_protocol(histProtocol, FIELD_CALCULATION_ID);

		std::vector<TextProtocol>history;
		while (true) {
			std::string received;
			if (receive_text_protocol(received)) {
				const TextProtocol receivedProtocol(received);
				if (receivedProtocol.operation != OP_ID_SESSION && receivedProtocol.status != STATUS_HISTORY_EMPTY) {
					history.push_back(receivedProtocol);
					//Odbieranie koñczy siê przy natrafieniu na numer sekwencyjny 0
					if (receivedProtocol.sequenceNumber == 0 && receivedProtocol.sessionId != 0) { break; }
				}
			}
		}

		print_message_sequence(history);

		CONSOLE_MANIP::cursor_set_pos(2, CONSOLE_MANIP::cursor_get_pos().Y + 1);
		CONSOLE_MANIP::press_any_key_pause();
	}

	//Funkcja wywo³uj¹ca wybieranie akcji i wykonuj¹ca wybór
	void action_choice() {
		sessionIdInfo = "Identyfikator sesji: " + std::to_string(sessionId);

		byte choice = 1;
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
				//Wys³anie pola operacja
				TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 1);
				send_text_protocol(statusProtocol, FIELD_STATUS);

				TextProtocol protocol(GET_CURRENT_TIME(), sessionId, 0);
				protocol.operation = OP_END;
				send_text_protocol(protocol, FIELD_OPERATION);
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
	static void action_choice_main_menu(byte& choice) {
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
		byte choice = 1;
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
				if (choice == 1) { goBackText[0] = '>'; }
				else if (choice == 2) { additionText[0] = '>'; }
				else if (choice == 3) { subtractionText[0] = '>'; }
				else if (choice == 4) { multiplicationText[0] = '>'; }
				else if (choice == 5) { divisionText[0] = '>'; }
				else if (choice == 6) { factorialText[0] = '>'; }

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
				if (CONSOLE_MANIP::check_escape()) { choice = 1; break; }
				else if (CONSOLE_MANIP::check_arrow("UP") && choice > 1) { choice--; }
				else if (CONSOLE_MANIP::check_arrow("DOWN") && choice < 6) { choice++; }
				else if (CONSOLE_MANIP::check_enter()) { break; }
			}

			//Przejœcie do wykonywania wybranego dzia³ania

			//Powrót
			if (choice == 1) { break; }
			//Dodawanie
			else if (choice == 2) {
				calculation(&arg_input_two_add, OP_ADD);
			}
			//Odejmowanie
			else if (choice == 3) {
				calculation(&arg_input_two_subt, OP_SUBT);
			}
			//Mno¿enie
			else if (choice == 4) {
				calculation(&arg_input_two_multp, OP_MULTP);
			}
			//Dzielenie
			else if (choice == 5) {
				calculation(&arg_input_two_div, OP_DIV);
			}
			//Silnia
			else if (choice == 6) {
				calculation(&arg_input_one_uint_fact, OP_FACT);
			}
		}
	}

	//Menu historii
	void history_menu() {
		byte choice = 1;
		std::string goBackText = " Powrót.";
		std::string wholeHistory = " Wyœwietl ca³¹ historiê.";
		std::string byCalcId = " Wyœwietl obliczenie o podanym identyfikatorze.";

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
				wholeHistory[0] = ' ';
				byCalcId[0] = ' ';
				if (choice == 1) { goBackText[0] = '>'; }
				else if (choice == 2) { wholeHistory[0] = '>'; }
				else if (choice == 3) { byCalcId[0] = '>'; }

				//Wyœwietlanie opcji
				CONSOLE_MANIP::print_text(2, 4, goBackText);
				CONSOLE_MANIP::print_text(2, 5, wholeHistory);
				CONSOLE_MANIP::print_text(2, 6, byCalcId);

				//Czyszczynie bufora wejœcia, aby po wduszeniu przycisku,
				// jego akcja nie zosta³a wielokrotnie wykonana
				CONSOLE_MANIP::clear_console_input_buffer();

				//Sprawdzanie naciœniêtych klawiszy
				if (CONSOLE_MANIP::check_escape()) { choice = 1; break; }
				else if (CONSOLE_MANIP::check_arrow("UP") && choice > 1) { choice--; }
				else if (CONSOLE_MANIP::check_arrow("DOWN") && choice < 3) { choice++; }
				else if (CONSOLE_MANIP::check_enter()) { break; }
			}

			//Przejœcie do wykonywania wybranej akcji

			//Powrót
			if (choice == 1) { break; }
			//Ca³a historia
			else if (choice == 2) {
				history_by_session_id();
			}
			//Po identyfikatorze obliczeñ
			else if (choice == 3) {
				history_by_calc_id();
			}
		}
	}

};
