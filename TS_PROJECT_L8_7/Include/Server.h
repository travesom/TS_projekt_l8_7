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
	static bool add(const double& argument1, const double& argument2, double& result) {
		const double tempResult = argument1 + argument2;
		if (tempResult <= 2147483647.0 && tempResult >= -2147483647.0) {
			result = tempResult;
			return true;
		}
		else { return false; }
	}

	//Odejmowanie
	static bool substract(const double& argument1, const double& argument2, double& result) {
		const double tempResult = argument1 - argument2;
		if (tempResult <= 2147483647.0 && tempResult >= -2147483647.0) {
			result = int(tempResult);
			return true;
		}
		else { return false; }
	}

	//Mno¿enie
	static bool multiply(const double& argument1, const double& argument2, double& result) {
		double tempResult = 0;
		for (int i = abs(int(argument2)); i > 0; i--) {
			tempResult += argument1;
			if (tempResult >= 2147483647.0 || tempResult <= -2147483647.0) {
				return false;
			}
		}
		result = argument1 * argument2;
		return true;
	}

	//Dzielenie
	static bool divide(const double& argument1, const double& argument2, double& result) {
		result = double(argument1) / double(argument2);
		result = round(result * 10000) / 10000;

		return true;
	}

	//Silnia
	static bool factorial(const double& argument1, const double& argument2, double& result) {
		//Silnia dla zero
		if (argument1 == 0) { result = 1; return true; }

		double temp = 1;
		for (unsigned int i = 1; i <= argument1; i++) {
			temp = temp * i;
			//Jeœli liczba poza zakresem
			if (temp > 4294967295.0) { return false; }
		}
		result = unsigned int(temp);
		return true;
	}

	//Obliczenia
	bool calculation(bool(*calc_function)(const double&, const double&, double&)) {
		std::string received;

		//Wys³anie identyfikatora obliczeñ
		TextProtocol calcIdProtocol(GET_CURRENT_TIME(), currentSessionId, "IDENTYFIKATOR_OBLICZEN");
		calcIdProtocol.calculationId = calculationId;
		send_text_protocol(calcIdProtocol, SEND_CALCULATION_ID);

		//Kontener na komunikaty potrzebne do obliczeñ
		const std::vector<TextProtocol> receivedMessages = receive_parts();

		//Przegl¹danie otrzymanych komunikatów
		std::array<double, 2> args{ 0.0,0.0 };
		unsigned int argNum = 0;
		for (const TextProtocol& prot : receivedMessages) {
			if (prot.operation == "ARGUMENT") {
				history[calculationId].second.push_back(prot);
				args[argNum] = prot.number;
				argNum++;
			}
		}

		double result; //Wynik

		//Jeœli silnia siê nie powiedzie
		if (!calc_function(args[0], args[1], result)) {
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
	void whole_history() {
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
				calculation(&add);
				calculationId++;
			}
			else if (operationProtocol.operation == "ODEJMOWANIE") {
				calculation(&substract);
				calculationId++;
			}
			else if (operationProtocol.operation == "MNOZENIE") {
				calculation(&multiply);
				calculationId++;
			}
			else if (operationProtocol.operation == "DZIELENIE") {
				calculation(&divide);
				calculationId++;
			}
			else if (operationProtocol.operation == "SILNIA") {
				calculation(&factorial);
				calculationId++;
			}
			else if (operationProtocol.operation == "HISTORIA_CALA") {
				whole_history();
			}
		}
	}
};
