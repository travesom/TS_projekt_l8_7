#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"
#include <random>
#include <ctime>
#include <unordered_map>
#include <array>


inline int randInt(const int &min, const int &max) {
	if (max <= min) return min;
	std::random_device rd;
	std::mt19937 gen(rd());
	const std::uniform_int_distribution<int > d(min, max);
	return d(gen);
}

class ServerUDP : public NodeUDP {
public:
	unsigned int currentSessionId;
	unsigned int calculationId = 1;

	/**
	 * Mapa przechowuj¹ca historie sesji. \n
	 * Kluczem jest identyfikator sesji, a wartoœci¹ tablica odebranych i wys³anych protoko³ów.
	*/
	std::unordered_map<unsigned int, std::pair<unsigned int, std::vector<TextProtocol>>> history;
	std::vector<unsigned int>sessionIds;

	//Konstruktor i destruktor
	ServerUDP(const u_long& IP, const unsigned short& Port1) : NodeUDP(IP, Port1) {
		const int iResult = bind(nodeSocket, reinterpret_cast<SOCKADDR *>(&nodeAddr), sizeof(nodeAddr));
		if (iResult != 0) {
			std::cout << "Bindowanie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return;
		}
	};
	virtual ~ServerUDP() { WSACleanup(); };

	bool start_session() {
		//Czekanie na ¿¹danie sesji
		std::string received;
		receive_text_protocol(received); //Moment czekania
		const TextProtocol receivedProt(received);

		//Wys³anie id
		unsigned int sessionId = receivedProt.id;
		//Jeœli klient nie ma identyfikatora sesji
		if (sessionId == 0) {
			while (true) {
				sessionId = randInt(1, 99);
				if (std::find(sessionIds.begin(), sessionIds.end(), sessionId) == sessionIds.end()) { break; }
			}
			sessionIds.push_back(sessionId);
		}

		currentSessionId = sessionId;

		const TextProtocol startProtocol(GET_CURRENT_TIME(), sessionId, "IDENTYFIKATOR_SESJI");

		if (!send_text_protocol(startProtocol, -1)) {
			std::cout << "B³¹d wysy³ania.\n";
			return false;
		}
		session();

		currentSessionId = 0;
		return true;
	}

	//Dodawanie (dziêki ma³emu oszustwu mo¿e byæ tak¿e odejmowaniem)
	bool add(const int& argument1, const int& argument2, int& result) {
		const long long int tempResult = argument1 + argument2;
		if (tempResult < 2147483647 && tempResult > -2147483647) {
			result = int(tempResult);
			return true;
		}
		else { return false; }
	}
	bool add_menu() {
		std::string received;

		//Wys³anie identyfikatora obliczeñ
		TextProtocol calcIdProtocol(GET_CURRENT_TIME(), currentSessionId, "IDENTYFIKATOR_OBLICZEN");
		calcIdProtocol.calculationId = calculationId;
		send_text_protocol(calcIdProtocol, SEND_CALCULATION_ID);

		//Kontener na komunikaty potrzebne do obliczeñ
		const std::vector<TextProtocol> receivedMessages = receive_parts();

		//Przegl¹danie otrzymanych komunikatów
		std::array<int, 2> args;
		unsigned int argNum = 0;
		for (const TextProtocol& prot : receivedMessages) {
			if (prot.operation == "ARGUMENT") {
				args[argNum] = int(prot.number);
				argNum++;
				//Dodanie do historii dla danego obliczenia
				history[calculationId].second.push_back(prot);
			}
		}

		int result; //Wynik
		//Jeœli silnia siê nie powiedzie
		if (!add(args[0], args[1], result)) {
			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 1);

			//Wysy³anie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, "STATUS");
			statusProtocol.status = STATUS_CALC_OUT_OF_RANGE;
			send_text_protocol(statusProtocol, SEND_STATUS);
			history[calculationId].second.push_back(statusProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 0);

			return false;
		}
		//Jeœli silnia siê powiedzie
		else {
			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 2);

			//Wysy³anie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, "STATUS");
			statusProtocol.status = STATUS_CALC_SUCCESS;
			send_text_protocol(statusProtocol, SEND_STATUS);
			history[calculationId].second.push_back(statusProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 1);

			//Wys³anie wyniku
			TextProtocol resultProtocol(GET_CURRENT_TIME(), currentSessionId, "WYNIK");
			resultProtocol.number = result;
			send_text_protocol(resultProtocol, SEND_NUMBER);
			history[calculationId].second.push_back(resultProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 0);
		}
		return true;
	}

	//Mno¿enie
	bool multiply(const int& argument1, const int& argument2, int& result) {
		long long tempResult = 0;
		for (int i = abs(argument2); i > 0; i--) {
			tempResult += argument1;
			if (tempResult > 2147483647 || tempResult < -2147483647) {
				return false;
			}
		}
		result = argument1 * argument2;
		return true;
	}
	bool multp_menu() {
		std::string received;

		//Wys³anie identyfikatora obliczeñ
		TextProtocol calcIdProtocol(GET_CURRENT_TIME(), currentSessionId, "IDENTYFIKATOR_OBLICZEN");
		calcIdProtocol.calculationId = calculationId;
		send_text_protocol(calcIdProtocol, SEND_CALCULATION_ID);

		//Kontener na komunikaty potrzebne do obliczeñ
		const std::vector<TextProtocol> receivedMessages = receive_parts();

		//Przegl¹danie otrzymanych komunikatów
		std::array<int, 2> args;
		unsigned int argNum = 0;
		for (const TextProtocol& prot : receivedMessages) {
			if (prot.operation == "ARGUMENT") {
				args[argNum] = int(prot.number);
				argNum++;
				history[calculationId].second.push_back(prot);
			}
		}

		int result; //Wynik
		//Jeœli mno¿enie siê nie powiedzie
		if (!multiply(args[0], args[1], result)) {
			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 1);

			//Wysy³anie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, "STATUS");
			statusProtocol.status = STATUS_CALC_OUT_OF_RANGE;
			send_text_protocol(statusProtocol, SEND_STATUS);
			history[calculationId].second.push_back(statusProtocol);

			//Wys³anie numeru sekwencyjnego (koñcowego)
			send_sequence_number(currentSessionId, 0);

			return false;
		}
		//Jeœli silnia siê powiedzie
		else {
			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 2);

			//Wysy³anie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, "STATUS");
			statusProtocol.status = STATUS_CALC_SUCCESS;
			send_text_protocol(statusProtocol, SEND_STATUS);
			history[calculationId].second.push_back(statusProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 1);

			//Wys³anie wyniku
			TextProtocol resultProtocol(GET_CURRENT_TIME(), currentSessionId, "WYNIK");
			resultProtocol.number = result;
			send_text_protocol(resultProtocol, SEND_NUMBER);
			history[calculationId].second.push_back(resultProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 0);
		}
		return true;
	}

	//Dzielenie
	bool divide(const int& argument1, const int& argument2, double& result) {
		result = double(argument1) / double(argument2);
		result = round(result * 100000000) / 100000000;

		return true;
	}
	bool div_menu() {
		std::string received;

		//Wys³anie identyfikatora obliczeñ
		TextProtocol calcIdProtocol(GET_CURRENT_TIME(), currentSessionId, "IDENTYFIKATOR_OBLICZEN");
		calcIdProtocol.calculationId = calculationId;
		send_text_protocol(calcIdProtocol, SEND_CALCULATION_ID);

		//Kontener na komunikaty potrzebne do obliczeñ
		const std::vector<TextProtocol> receivedMessages = receive_parts();

		//Przegl¹danie otrzymanych komunikatów
		std::array<int, 2> args;
		unsigned int argNum = 0;
		for (const TextProtocol& prot : receivedMessages) {
			if (prot.operation == "ARGUMENT") {
				args[argNum] = int(prot.number);
				argNum++;
				history[calculationId].second.push_back(prot);
			}
		}

		double result; //Wynik
		//Jeœli mno¿enie siê nie powiedzie
		if (!divide(args[0], args[1], result)) {
			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 1);

			//Wysy³anie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, "STATUS");
			statusProtocol.status = STATUS_CALC_OUT_OF_RANGE;
			send_text_protocol(statusProtocol, SEND_STATUS);
			history[calculationId].second.push_back(statusProtocol);

			//Wys³anie numeru sekwencyjnego (koñcowego)
			send_sequence_number(currentSessionId, 0);

			return false;
		}
		//Jeœli silnia siê powiedzie
		else {
			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 2);

			//Wysy³anie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, "STATUS");
			statusProtocol.status = STATUS_CALC_SUCCESS;
			send_text_protocol(statusProtocol, SEND_STATUS);
			history[calculationId].second.push_back(statusProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 1);

			//Wys³anie wyniku
			TextProtocol resultProtocol(GET_CURRENT_TIME(), currentSessionId, "WYNIK");
			resultProtocol.number = result;
			send_text_protocol(resultProtocol, SEND_NUMBER);
			history[calculationId].second.push_back(resultProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 0);
		}
		return true;
	}

	//SILNIA
	bool factorial(const unsigned int& argument, unsigned int& result) {
		//Silnia dla zero
		if (argument == 0) { result = 1; return true; }

		long long unsigned int temp = 1;
		for (unsigned int i = 1; i <= argument; i++) {
			temp = temp * i;
			//Jeœli liczba poza zakresem
			if (temp > 4294967295) { return false; }
		}
		result = unsigned int(temp);
		return true;
	}
	bool factorial_menu() {
		std::string received;

		//Wys³anie identyfikatora obliczeñ
		TextProtocol calcIdProtocol(GET_CURRENT_TIME(), currentSessionId, "IDENTYFIKATOR_OBLICZEN");
		calcIdProtocol.calculationId = calculationId;
		send_text_protocol(calcIdProtocol, SEND_CALCULATION_ID);

		//Kontener na komunikaty potrzebne do obliczeñ
		const std::vector<TextProtocol> receivedMessages = receive_parts();

		//Przegl¹danie otrzymanych komunikatów
		unsigned int arg = 0;
		for (const TextProtocol& prot : receivedMessages) {
			if (prot.operation == "ARGUMENT") {
				history[calculationId].second.push_back(prot);
				arg = unsigned int(prot.number);
			}
		}

		unsigned int result; //Wynik
		//Jeœli silnia siê nie powiedzie
		if (!factorial(arg, result)) {
			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 1);

			//Wysy³anie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, "STATUS");
			statusProtocol.status = STATUS_CALC_OUT_OF_RANGE;
			send_text_protocol(statusProtocol, SEND_STATUS);
			history[calculationId].second.push_back(statusProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 0);

			return false;
		}
		//Jeœli silnia siê powiedzie
		else {
			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 2);

			//Wysy³anie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, "STATUS");
			statusProtocol.status = STATUS_CALC_SUCCESS;
			send_text_protocol(statusProtocol, SEND_STATUS);
			history[calculationId].second.push_back(statusProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 1);

			//Wys³anie wyniku
			TextProtocol resultProtocol(GET_CURRENT_TIME(), currentSessionId, "WYNIK");
			resultProtocol.number = result;
			send_text_protocol(resultProtocol, SEND_NUMBER);
			history[calculationId].second.push_back(resultProtocol);

			//Wys³anie numeru sekwencyjnego
			send_sequence_number(currentSessionId, 0);
		}
		return true;
	}

	//Historia
	std::vector<TextProtocol> get_history_by_session_id(const unsigned int& sessionId) {
		std::vector<TextProtocol> result;

		for (const auto& elem : history) {
			if (elem.second.first == sessionId) {
				for (const auto& prot : elem.second.second) {
					result.push_back(prot);
				}
			}
		}
		return result;
	}

	bool session() {
		while (true) {
			TextProtocol operationProtocol;
			std::string received;

			receive_text_protocol(received);
			operationProtocol.from_string(received);
			if (operationProtocol.operation != "HISTORIA_CALA" && operationProtocol.operation != "ZAKONCZENIE") {
				std::cout << "Received (session): " << received << '\n';
				history[calculationId].first = currentSessionId;
				history[calculationId].second.push_back(operationProtocol);
			}

			if (operationProtocol.operation == "ZAKONCZENIE") {//sprawdza czy klient chce sie rozl¹czyæ
				return true;
			}
			else if (operationProtocol.operation == "DODAWANIE") {
				add_menu();
				calculationId++;
			}
			else if (operationProtocol.operation == "ODEJMOWANIE") {
				add_menu();
				calculationId++;
			}
			else if (operationProtocol.operation == "MNOZENIE") {
				multp_menu();
				calculationId++;
			}
			else if (operationProtocol.operation == "DZIELENIE") {
				div_menu();
				calculationId++;
			}
			else if (operationProtocol.operation == "SILNIA") {
				factorial_menu();
				calculationId++;
			}
			else if (operationProtocol.operation == "HISTORIA_CALA") {
				unsigned int sequenceNumber = 0;
				std::vector<TextProtocol> sessionHistory = get_history_by_session_id(currentSessionId);
				send_sequence_number(currentSessionId, sessionHistory.size() - sequenceNumber);
				while (true) {
					int field = SEND_NO_ADDITIONAL;
					if (sessionHistory[sequenceNumber].operation == "STATUS") { field = SEND_STATUS; }
					else if (sessionHistory[sequenceNumber].operation == "ARGUMENT") { field = SEND_NUMBER; }
					else if (sessionHistory[sequenceNumber].operation == "WYNIK") { field = SEND_NUMBER; }

					std::cout << sessionHistory[sequenceNumber].to_string(field) << '\n';
					send_text_protocol(sessionHistory[sequenceNumber], field);

					sequenceNumber++;
					send_sequence_number(currentSessionId, sessionHistory.size() - sequenceNumber);
					if (sequenceNumber == sessionHistory.size()) { break; }
				}
			}
		}
	}
};
