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

	//Rozpoczêcie sesji
	bool start_session() {
		//Czekanie na ¿¹danie sesji
		std::string received;
		TextProtocol receivedProt;

		//Czekanie na ¿¹danie rozpoczêcia sesji
		while (receivedProt.operation != OP_BEGIN) {
			receive_text_protocol(received);
			receivedProt = TextProtocol(received);
		}

		//Wybieranie id sesji
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

		TextProtocol startProtocol(GET_CURRENT_TIME(), sessionId, 0);
		startProtocol.operation = OP_ID_SESSION;

		//Wys³anie id
		if (!send_text_protocol(startProtocol, FIELD_OPERATION)) {
			std::cout << "B³¹d wysy³ania.\n";
			return false;
		}
		else { session(); }

		currentSessionId = 0;
		return true;
	}

	//Dodawanie
	static bool add(const double& argument1, const double& argument2, double& result) {
		const double tempResult = argument1 + argument2;
		if (tempResult <= 2147483647.0 && tempResult >= -2147483647.0) {
			result = tempResult;
			return true;
		}
		else { return false; }
	}

	//Odejmowanie
	static bool subtract(const double& argument1, const double& argument2, double& result) {
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

	//Obliczenia (typ obliczeñ okreœlany poprzez wskaŸnik funkcji)
	void calculation(bool(*calc_function)(const double&, const double&, double&)) {
		std::string received;

		//Kontener na komunikaty potrzebne do obliczeñ
		const std::vector<TextProtocol> receivedMessages = receive_parts();

		//Przegl¹danie otrzymanych komunikatów
		std::array<double, 2> args{ 0.0,0.0 };
		unsigned int argNum = 0;
		for (const TextProtocol& prot : receivedMessages) {
			if (!isnan(prot.number)) {
				history[calculationId].second.push_back(prot);
				args[argNum] = prot.number;
				argNum++;
			}
		}

		double result; //Wynik

		//Kontener na komunikaty dotycz¹ce wyniku obliczeñ
		std::vector<TextProtocol> resultMessages;
		TextProtocol resultMessage(GET_CURRENT_TIME(), currentSessionId, 0);
		resultMessage.operation = OP_RESULT;
		history[calculationId].second.push_back(resultMessage);
		resultMessages.push_back(resultMessage);

		//Jeœli silnia siê nie powiedzie
		if (!calc_function(args[0], args[1], result)) {
			//Dodanie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, 0);
			statusProtocol.status = STATUS_OUT_OF_RANGE;
			history[calculationId].second.push_back(statusProtocol);
			resultMessages.push_back(statusProtocol);
		}
		//Jeœli silnia siê powiedzie
		else {
			//Dodanie statusu
			TextProtocol statusProtocol(GET_CURRENT_TIME(), currentSessionId, 0);
			statusProtocol.status = STATUS_SUCCESS;
			history[calculationId].second.push_back(statusProtocol);
			resultMessages.push_back(statusProtocol);

			//Dodanie wyniku
			TextProtocol resultProtocol(GET_CURRENT_TIME(), currentSessionId, 0);
			resultProtocol.number = result;
			history[calculationId].second.push_back(resultProtocol);
			resultMessages.push_back(resultProtocol);
		}

		//Dodanie identyfikatora obliczeñ
		TextProtocol calcIdProtocol(GET_CURRENT_TIME(), currentSessionId, 0);
		calcIdProtocol.calculationId = calculationId;
		history[calculationId].second.push_back(calcIdProtocol);
		resultMessages.push_back(calcIdProtocol);

		unsigned int sequenceNumber = resultMessages.size() - 1;
		std::cout << "\nWysy³anie wyniku...\n";
		for (TextProtocol prot : resultMessages) {
			unsigned int sendField = -1;
			prot.sequenceNumber = sequenceNumber;
			std::cout << "Sent: " << prot.to_string(prot.get_field()) << '\n';
			send_text_protocol(prot, prot.get_field());
			sequenceNumber--;
		}
	}

	//Historia (dla id sesji)
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
		std::vector<TextProtocol> sessionHistory = get_history_by_session_id(currentSessionId);
		unsigned int sequenceNumber = sessionHistory.size() - 1;
		unsigned int historyIterator = 0;
		while (true) {
			std::cout << sessionHistory[historyIterator].to_string(sessionHistory[historyIterator].get_field()) << '\n';
			sessionHistory[historyIterator].sequenceNumber = sequenceNumber;
			send_text_protocol(sessionHistory[historyIterator], sessionHistory[historyIterator].get_field());

			sequenceNumber--;
			historyIterator++;
			if (sequenceNumber == -1) { break; }
		}
	}

	bool session() {
		while (true) {
			TextProtocol operationProtocol;
			std::string received;

			receive_text_protocol(received);
			operationProtocol.from_string(received);

			if (operationProtocol.operation == OP_END) {//sprawdza czy klient chce sie rozl¹czyæ
				return true;
			}

			if (operationProtocol.operation != OP_HISTORY_WHOLE) {
				std::cout << "Received (session): " << received << '\n';
				history[calculationId].first = currentSessionId;
				history[calculationId].second.push_back(operationProtocol);
			}
			if (operationProtocol.operation == OP_ADD) {
				calculation(&add);
				calculationId++;
			}
			else if (operationProtocol.operation == OP_SUBT) {
				calculation(&subtract);
				calculationId++;
			}
			else if (operationProtocol.operation == OP_MULTP) {
				calculation(&multiply);
				calculationId++;
			}
			else if (operationProtocol.operation == OP_DIV) {
				calculation(&divide);
				calculationId++;
			}
			else if (operationProtocol.operation == OP_FACT) {
				calculation(&factorial);
				calculationId++;
			}
			else if (operationProtocol.operation == OP_HISTORY_WHOLE) {
				whole_history();
			}
		}
	}
};
