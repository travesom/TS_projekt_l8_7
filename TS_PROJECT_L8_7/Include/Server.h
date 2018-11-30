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
	unsigned int calculationId = 1;

	/**
	 * Mapa przechowuj�ca historie sesji. \n
	 * Kluczem jest identyfikator sesji, a warto�ci� tablica odebranych i wys�anych protoko��w.
	*/
	std::unordered_map<unsigned int, std::pair<unsigned int, std::vector<TextProtocol>>> history;
	std::vector<unsigned int>sessionIds;

	//Konstruktor i destruktor
	ServerUDP(const u_long& IP, const unsigned short& Port1) : NodeUDP(IP, Port1) {
		//Bindowanie gniazdka dla adresu odbierania
		const int iResult = bind(nodeSocket, reinterpret_cast<SOCKADDR *>(&nodeAddr), sizeof(nodeAddr));
		if (iResult != 0) {
			std::cout << "Bindowanie niepowiod�o si� z b��dem: " << WSAGetLastError() << "\n";
			return;
		}
	};

	//Rozpocz�cie sesji
	bool start_session() {
		//Czekanie na ��danie sesji
		std::string received;
		TextProtocol receivedProt;

		//Czekanie na ��danie rozpocz�cia sesji
		while (receivedProt.operation != OP_BEGIN) {
			receive_text_protocol(received);
			receivedProt = TextProtocol(received);
		}

		//Wybieranie id sesji
		unsigned int sessionId = receivedProt.id;
		//Je�li klient nie ma identyfikatora sesji
		if (sessionId == 0) {
			while (true) {
				sessionId = randInt(1, 99);
				if (std::find(sessionIds.begin(), sessionIds.end(), sessionId) == sessionIds.end()) { break; }
			}
			sessionIds.push_back(sessionId);
		}

		//Wys�anie id
		TextProtocol startProtocol(GET_CURRENT_TIME(), sessionId, 0);
		startProtocol.operation = OP_ID_SESSION;
		if (!send_text_protocol(startProtocol, FIELD_OPERATION)) {
			std::cout << "B��d wysy�ania.\n";
			return false;
		}
		else { session(); }

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

	//Mno�enie
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
			//Je�li liczba poza zakresem
			if (temp > 4294967295.0) { return false; }
		}
		result = unsigned int(temp);
		return true;
	}

	//Obliczenia (typ oblicze� okre�lany poprzez wska�nik funkcji)
	void calculation(bool(*calc_function)(const double&, const double&, double&), const unsigned int& sessionId) {
		//Kontener na komunikaty potrzebne do oblicze�
		const std::vector<TextProtocol> receivedMessages = receive_messages();

		//Przegl�danie otrzymanych komunikat�w
		std::array<double, 2> args{ 0.0,0.0 };
		unsigned int argNum = 0;
		for (const TextProtocol& prot : receivedMessages) {
			if (prot.get_field() == FIELD_NUMBER) {
				history[calculationId].second.push_back(prot);
				args[argNum] = prot.number;
				argNum++;
			}
		}

		double result; //Wynik

		//Kontener na komunikaty dotycz�ce wyniku oblicze�
		std::vector<TextProtocol> resultMessages;

		//Dodanie operacji WYNIK do historii i do komunikat�w do odes�ania
		TextProtocol resultMessage(GET_CURRENT_TIME(), sessionId, 0);
		resultMessage.operation = OP_STATUS;
		history[calculationId].second.push_back(resultMessage);
		resultMessages.push_back(resultMessage);

		//Je�li obliczenie si� nie powiedzie
		if (!calc_function(args[0], args[1], result)) {
			//Dodanie statusu do historii i do komunikat�w do odes�ania
			TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
			statusProtocol.status = STATUS_OUT_OF_RANGE;
			history[calculationId].second.push_back(statusProtocol);
			resultMessages.push_back(statusProtocol);
		}
		//Je�li obliczenie si� powiedzie
		else {
			//Dodanie statusu do historii i do komunikat�w do odes�ania
			TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
			statusProtocol.status = STATUS_SUCCESS;
			history[calculationId].second.push_back(statusProtocol);
			resultMessages.push_back(statusProtocol);

			//Dodanie wyniku do historii i do komunikat�w do odes�ania
			TextProtocol resultProtocol(GET_CURRENT_TIME(), sessionId, 0);
			resultProtocol.number = result;
			history[calculationId].second.push_back(resultProtocol);
			resultMessages.push_back(resultProtocol);
		}

		//Dodanie identyfikatora oblicze� do historii i do komunikat�w do odes�ania
		TextProtocol calcIdProtocol(GET_CURRENT_TIME(), sessionId, 0);
		calcIdProtocol.calculationId = calculationId;
		//Dodanie do historii u�atwia p�niej wysy�anie historii
		history[calculationId].second.push_back(calcIdProtocol);
		resultMessages.push_back(calcIdProtocol);

		//Wysy�anie wyniku
		std::cout << "\nWysy�anie wyniku...\n";
		unsigned int sequenceNumber = resultMessages.size() - 1;
		for (TextProtocol prot : resultMessages) {
			prot.sequenceNumber = sequenceNumber;
			std::cout << "Sent: " << prot.to_string(prot.get_field()) << '\n';
			send_text_protocol(prot, prot.get_field());
			sequenceNumber--;
		}
	}

	//Historia (dla id sesji)
	std::vector<TextProtocol> get_history_by_session_id(const unsigned int& sessionId) {
		//Kontener do przechowywania komunikat�w dla danego id sesji
		std::vector<TextProtocol> result;

		//Przegl�danie historii
		for (const auto& elem : history) {
			if (elem.second.first == sessionId) {
				for (const auto& prot : elem.second.second) {
					result.push_back(prot);
				}
			}
		}
		return result;
	}
	void history_by_session_id(const unsigned int& sessionId) {
		if (!history.empty()) {
			//Kontener z histori� dla danego id sesji
			std::vector<TextProtocol> sessionHistory = get_history_by_session_id(sessionId);

			TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
			statusProtocol.status = STATUS_SUCCESS;
			sessionHistory.insert(sessionHistory.begin(), statusProtocol);

			//Numer sekwencyjny dla wysy�anych komunikat�w
			unsigned int sequenceNumber = sessionHistory.size() - 1;
			//Zmienna u�ywana do iterowania po historii
			for (TextProtocol prot : sessionHistory) {
				//Wys�anie komunikatu
				prot.sequenceNumber = sequenceNumber;
				std::cout << prot.to_string(prot.get_field()) << '\n';

				send_text_protocol(prot, prot.get_field());

				//Zmniejszenie numeru sekwencyjnego i zwi�kszenie iteratora po historii
				sequenceNumber--;
				if (sequenceNumber < 0) { break; }
			}
		}
		else {
			TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
			statusProtocol.status = STATUS_HISTORY_EMPTY;
			send_text_protocol(statusProtocol, statusProtocol.get_field());
			std::cout << "Sent: " << statusProtocol.to_string(statusProtocol.get_field()) << '\n';
		}
	}

	//Historia identyfikatorze oblicze�
	void history_by_calc_id(const unsigned int& sessionId, const unsigned int& calcId) {
		if (!history.empty()) {
			if (history.find(calcId) != history.end()) {
				if (history[calcId].first != sessionId) {
					TextProtocol operationProtocol(GET_CURRENT_TIME(), sessionId, 1);
					operationProtocol.operation = OP_STATUS;
					send_text_protocol(operationProtocol, operationProtocol.get_field());

					TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
					statusProtocol.status = STATUS_FORBIDDEN;
					send_text_protocol(statusProtocol, statusProtocol.get_field());
				}
				else if (history[calcId].first == sessionId) {
					int sequenceNumber = history[calcId].second.size() + 1;

					TextProtocol operationProtocol(GET_CURRENT_TIME(), sessionId, sequenceNumber);
					operationProtocol.operation = OP_STATUS;
					send_text_protocol(operationProtocol, operationProtocol.get_field());
					sequenceNumber--;

					TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, sequenceNumber);
					statusProtocol.status = STATUS_SUCCESS;
					send_text_protocol(statusProtocol, statusProtocol.get_field());
					sequenceNumber--;

					for (TextProtocol prot : history[calcId].second) {
						prot.sequenceNumber = sequenceNumber;
						send_text_protocol(prot, prot.get_field());
						sequenceNumber--;
						if (sequenceNumber < 0) { break; }
					}
				}
			}
			else {
				TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
				statusProtocol.status = STATUS_NOT_FOUND;
				send_text_protocol(statusProtocol, statusProtocol.get_field());
				std::cout << "Sent: " << statusProtocol.to_string(statusProtocol.get_field()) << '\n';
			}
		}
		else {
			TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
			statusProtocol.status = STATUS_HISTORY_EMPTY;
			send_text_protocol(statusProtocol, statusProtocol.get_field());
			std::cout << "Sent: " << statusProtocol.to_string(statusProtocol.get_field()) << '\n';
		}
	}

	bool session() {
		//P�tla g��wna sesji
		while (true) {
			std::string received;
			receive_text_protocol(received);
			TextProtocol operationProtocol(received);

			if (operationProtocol.get_field() == FIELD_OPERATION) {
				std::cout << "Received (session): " << received << '\n';
				//Operacje nie do oblicze� -------------------------------------------------------------

				//Zako�czenie
				if (operationProtocol.operation == OP_END) {//sprawdza czy klient chce sie rozl�czy�
					return true;
				}
				//Wy�wietlenie ca�ej historii dla obecnej
				else if (operationProtocol.operation == OP_HISTORY_WHOLE) {
					history_by_session_id(operationProtocol.id);
					continue;
				}
				else if (operationProtocol.operation == OP_HISTORY_ID) {
					receive_text_protocol(received);
					TextProtocol idProtocol(received);
					history_by_calc_id(operationProtocol.id, idProtocol.calculationId);
					continue;
				}

				//Operacje do oblicze� -----------------------------------------------------------------

				//Wpisanie operacji do historii
				history[calculationId].first = operationProtocol.id;
				history[calculationId].second.push_back(operationProtocol);

				//Dodawanie
				if (operationProtocol.operation == OP_ADD) { calculation(&add, operationProtocol.id); }
				//Odejmowanie
				else if (operationProtocol.operation == OP_SUBT) { calculation(&subtract, operationProtocol.id); }
				//Mno�enie
				else if (operationProtocol.operation == OP_MULTP) { calculation(&multiply, operationProtocol.id); }
				//Dzielenie
				else if (operationProtocol.operation == OP_DIV) { calculation(&divide, operationProtocol.id); }
				//Silnia
				else if (operationProtocol.operation == OP_FACT) { calculation(&factorial, operationProtocol.id); }

				//Zwi�kszenie id oblicze�
				calculationId++;
			}
		}
	}
};
