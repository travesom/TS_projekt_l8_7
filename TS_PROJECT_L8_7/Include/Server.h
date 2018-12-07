#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"
#include <random>
#include <map>
#include <array>


inline int randInt(const int &min, const int &max) {
	if (max <= min) return min;
	std::random_device rd;
	std::mt19937 gen(rd());
	const std::uniform_int_distribution<int > d(min, max);
	return d(gen);
}

class ServerUDP : public NodeUDP {
private:
	unsigned int calculationId = 1; //Id obecnego obliczenia

	/**
		* Mapa przechowuj¹ca historiê obliczeñ. \n
		* Kluczem jest identyfikator obliczeñ, a wartoœci¹ para identyfikator sesji, komunikaty obliczeñ.
	*/
	std::map<unsigned int, std::pair<unsigned int, std::vector<TextProtocol>>> history;
	std::set<unsigned int>sessionIds; //Zbiór u¿ywanych identyfikatorów sesji
	const unsigned short port; //Port (zmienna u¿ywania przy bindowaniu dla pêtli lokalnej)
	sockaddr_in serverAddr{}; //Adress serwera (u¿ywany przy bindowaniu)


public:
	//Konstruktor
	explicit ServerUDP(const unsigned short& Port1) : NodeUDP(Port1), port(htons(Port1)) {
		messages = false;
		serverAddr.sin_family = AF_INET;
		bind_to_address("0.0.0.0");
		set_receive_timeout(recvTimeout);
	};

	//Funkcja rozpoczynaj¹ca sesjê
	bool start_session() {
		//Czekanie na ¿¹danie rozpoczêcia sesji
		bool sessionResult = false;
		if (listen_for_client()) { sessionResult = session(); }

		bind_to_address("0.0.0.0");

		if (!sessionResult) { return false; }
		else { return true; }
	}


private:
	//Bindowanie pod dany adres
	bool bind_to_address(const std::string& address) {
		serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
		if (address == "127.0.0.1") {
			serverAddr.sin_port = port + 1;
		}
		else { serverAddr.sin_port = port; }

		closesocket(nodeSocket);
		nodeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		sync_cout << "Bindowanie dla adresu: " << inet_ntoa(serverAddr.sin_addr) << " : " << serverAddr.sin_port << '\n';
		Sleep(100);
		const int iResult = bind(nodeSocket, reinterpret_cast<SOCKADDR *>(&serverAddr), sizeof(serverAddr));
		set_receive_timeout(1000);
		if (iResult != 0) {
			sync_cout << "Bindowanie niepowiod³o siê z b³êdem: " << WSAGetLastError() << "\n";
			return false;
		}
		else {
			sync_cout << "Bindowanie powiod³o siê\n";
		}
		return true;
	}

	//Przyjmowanie zg³oszenia
	bool listen_for_client() {
		unsigned int sessionId = 0;
		std::string received;
		TextProtocol receivedProt;

		sync_cout << "\nNas³uchiwanie na klientów.\n";
		byte failCount = 0;
	listening:
		while (true) {
			if (receive_text_protocol(received)) {
				sync_cout << "Odbieranie (nas³uchiwanie): " << received << '\n';
				receivedProt = TextProtocol(received);

				if (receivedProt.operation == OP_BEGIN) {/*nic*/ }
				else if (receivedProt.operation == OP_ACK) {
					sync_cout << "Rozpoczynanie sesji zakoñczone powodzeniem.\n\n";
					return true;
				}

				//Jeœli odebrano komunikat z adresem serwera
				if (receivedProt.get_field() == FIELD_ADDRESS) {
					if (!bind_to_address(receivedProt.address)) {
						sync_cout << "Rozpoczynanie sesji zakoñczone niepowodzeniem.\n\n";
						return false;
					}
				}
				if (receivedProt.sequenceNumber == 0) { break; }
			}
			else {
				failCount++;
				sync_cout << "Pozosta³o prób: " << 11 - failCount << '\n';
			}
			if (failCount == 11) { break; }
		}

		if (failCount < 11) {
			//Wybieranie sessionId sesji
			if (sessionId == 0) { sessionId = receivedProt.sessionId; }
			//Jeœli klient nie ma identyfikatora sesji (na serwerze)
			if (sessionIds.find(sessionId) == sessionIds.end() && sessionId == 0) {
				sessionId = randInt(1, 999);
				while (true) {
					if (sessionIds.find(sessionId) == sessionIds.end()) { break; }
					sessionId++;
					if (sessionId > 999) { sessionId = randInt(1, 999); }
				}
			}




			//Odbieranie potwierdzenia od klienta
			failCount = 0;
			while (receivedProt.operation != OP_ACK) {
				Sleep(200);
				//Wys³anie pola operacja
				TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 1);
				send_text_protocol(statusProtocol, FIELD_STATUS);

				//Wys³anie identyfikatora sesji (czêœæ jest w pêtli)
				TextProtocol idProtocol(GET_CURRENT_TIME(), sessionId, 0);
				idProtocol.operation = OP_ID_SESSION;
				Sleep(200);
				send_text_protocol(idProtocol, FIELD_OPERATION);

				sync_cout << "Wysy³anie (id): " << idProtocol.to_string(idProtocol.get_field()) << '\n';
				sync_cout << "Czekanie na potwierdzenie...\n";
				if (receive_text_protocol(received)) {
					sync_cout << "Odbieranie (potwierdzenie): " << received << '\n';
					receivedProt = TextProtocol(received);
					if (receivedProt.get_field() == FIELD_OPERATION && receivedProt.operation == OP_ACK) {
						sessionIds.insert(sessionId);
						std::cout << "U¿ywane identyfikatory sesji:\n";
						for(const unsigned int& id : sessionIds){
							std::cout << " - " << id << '\n';
						}
						sync_cout << "Rozpoczynanie sesji zakoñczone powodzeniem.\n\n";
						return true;
					}
					else if (failCount == 10) {
						bind_to_address("0.0.0.0");
						goto listening;
					}
				}
				failCount++;
			}
		}
		sync_cout << "Rozpoczynanie sesji zakoñczone niepowodzeniem.\n\n";
		return false;
	}



	//Funkcje obliczeñ ---------------------------------------------------------------------

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

	//Silnia (argument drugi obecny, aby mo¿na przekazaæ wskaŸnik tej funkcji)
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
	void calculation(bool(*calc_function)(const double&, const double&, double&), const unsigned int& sessionId) {
		//Kontener na komunikaty potrzebne do obliczeñ
		const std::vector<TextProtocol> receivedMessages = receive_messages();

		//Przegl¹danie otrzymanych komunikatów
		std::array<double, 2> args{ 0.0,0.0 };
		unsigned int argNum = 0;
		for (const TextProtocol& prot : receivedMessages) {
			if (prot.get_field() == FIELD_NUMBER) {
				history[calculationId].second.push_back(prot);
				args[argNum] = prot.number;
				argNum++;
			}
		}

		//Wynik
		double result;

		//Kontener na komunikaty dotycz¹ce wyniku obliczeñ
		std::vector<TextProtocol> resultMessages;

		//Dodanie operacji WYNIK do historii i do komunikatów do odes³ania
		TextProtocol resultMessage(GET_CURRENT_TIME(), sessionId, 0);
		resultMessage.operation = OP_STATUS;
		history[calculationId].second.push_back(resultMessage);
		resultMessages.push_back(resultMessage);

		//Jeœli obliczenie siê nie powiedzie
		if (!calc_function(args[0], args[1], result)) {
			//Dodanie statusu do historii i do komunikatów do odes³ania
			TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
			statusProtocol.status = STATUS_OUT_OF_RANGE;
			history[calculationId].second.push_back(statusProtocol);
			resultMessages.push_back(statusProtocol);
		}
		//Jeœli obliczenie siê powiedzie
		else {
			//Dodanie statusu do historii i do komunikatów do odes³ania
			TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
			statusProtocol.status = STATUS_SUCCESS;
			history[calculationId].second.push_back(statusProtocol);
			resultMessages.push_back(statusProtocol);

			//Dodanie wyniku do historii i do komunikatów do odes³ania
			TextProtocol resultProtocol(GET_CURRENT_TIME(), sessionId, 0);
			resultProtocol.number = result;
			history[calculationId].second.push_back(resultProtocol);
			resultMessages.push_back(resultProtocol);
		}

		//Dodanie identyfikatora obliczeñ do historii i do komunikatów do odes³ania
		TextProtocol calcIdProtocol(GET_CURRENT_TIME(), sessionId, 0);
		calcIdProtocol.calculationId = calculationId;
		//Dodanie do historii u³atwia póŸniej wysy³anie historii
		history[calculationId].second.push_back(calcIdProtocol);
		resultMessages.push_back(calcIdProtocol);

		//Wysy³anie wyniku
		sync_cout << "\nWysy³anie wyniku...\n";
		unsigned int sequenceNumber = resultMessages.size() - 1;
		for (TextProtocol prot : resultMessages) {
			prot.sequenceNumber = sequenceNumber;
			sync_cout << "Wysy³anie (obliczenia): " << prot.to_string(prot.get_field()) << '\n';
			send_text_protocol(prot, prot.get_field());
			sequenceNumber--;
		}
	}

	//--------------------------------------------------------------------------------------



	//Historia (dla sessionId sesji)
	std::vector<TextProtocol> get_history_by_session_id(const unsigned int& sessionId) {
		//Kontener do przechowywania komunikatów dla danego sessionId sesji
		std::vector<TextProtocol> result;

		//Przegl¹danie historii
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
			//Kontener z histori¹ dla danego sessionId sesji
			std::vector<TextProtocol> sessionHistory = get_history_by_session_id(sessionId);

			if (!sessionHistory.empty()) {
				TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, sessionHistory.size());
				statusProtocol.status = STATUS_FOUND;
				sessionHistory.insert(sessionHistory.begin(), statusProtocol);

				//Numer sekwencyjny dla wysy³anych komunikatów
				int sequenceNumber = sessionHistory.size() - 1;
				//Zmienna u¿ywana do iterowania po historii
				for (TextProtocol prot : sessionHistory) {
					//Wys³anie komunikatu
					prot.sequenceNumber = sequenceNumber;
					sync_cout << prot.to_string(prot.get_field()) << '\n';

					send_text_protocol(prot, prot.get_field());

					//Zmniejszenie numeru sekwencyjnego i zwiêkszenie iteratora po historii
					sequenceNumber--;
					if (sequenceNumber < 0) { break; }
				}
				return;
			}
		}
		//Dla niepowodzeñ wyœlij historia pusta
		TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
		statusProtocol.status = STATUS_HISTORY_EMPTY;
		send_text_protocol(statusProtocol, statusProtocol.get_field());
		sync_cout << "Wysy³anie (historia): " << statusProtocol.to_string(statusProtocol.get_field()) << '\n';
	}

	//Historia identyfikatorze obliczeñ
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
					sync_cout << "Wysy³anie (historia): " << statusProtocol.to_string(statusProtocol.get_field()) << '\n';
					return;
				}
				else if (history[calcId].first == sessionId) {
					int sequenceNumber = history[calcId].second.size() + 1;

					TextProtocol operationProtocol(GET_CURRENT_TIME(), sessionId, sequenceNumber);
					operationProtocol.operation = OP_STATUS;
					send_text_protocol(operationProtocol, operationProtocol.get_field());
					sequenceNumber--;

					TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, sequenceNumber);
					statusProtocol.status = STATUS_FOUND;
					send_text_protocol(statusProtocol, statusProtocol.get_field());
					sequenceNumber--;

					for (TextProtocol prot : history[calcId].second) {
						prot.sequenceNumber = sequenceNumber;
						sync_cout << "Wysy³anie (historia): " << prot.to_string(prot.get_field()) << '\n';
						send_text_protocol(prot, prot.get_field());
						sequenceNumber--;
						if (sequenceNumber < 0) { break; }
					}
					return;
				}
			}
		}

		TextProtocol statusProtocol(GET_CURRENT_TIME(), sessionId, 0);
		statusProtocol.status = STATUS_NOT_FOUND;
		send_text_protocol(statusProtocol, statusProtocol.get_field());
		sync_cout << "Wysy³anie (historia): " << statusProtocol.to_string(statusProtocol.get_field()) << '\n';
	}

	//W tej funkcji znajduje siê pêtla sesji
	bool session() {
		//Pêtla g³ówna sesji
		while (true) {
			std::string received;
			receive_text_protocol(received); //Komunikat z pustym statusem
			if (receive_text_protocol(received)) {
				TextProtocol operationProtocol(received);

				if (operationProtocol.get_field() == FIELD_OPERATION) {
					sync_cout << "Odbieranie (sesja): " << received << '\n';
					//Operacje nie do obliczeñ -------------------------------------------------------------

					//Zakoñczenie
					if (operationProtocol.operation == OP_END) { //Sprawdza czy klient chce sie rozl¹czyæ
						return true;
					}
					else if (operationProtocol.operation == OP_BEGIN) {
						return false;
					}
					//Wyœwietlenie ca³ej historii dla obecnej sesji
					else if (operationProtocol.operation == OP_HISTORY_WHOLE) {
						history_by_session_id(operationProtocol.sessionId);
						continue;
					}
					//Wyœwietlenie historii dla podanego identyfikatora obliczeñ
					else if (operationProtocol.operation == OP_HISTORY_ID) {
						receive_text_protocol(received);
						TextProtocol idProtocol(received);
						history_by_calc_id(operationProtocol.sessionId, idProtocol.calculationId);
						continue;
					}

					//Operacje do obliczeñ -----------------------------------------------------------------

					//Wpisanie operacji do historii
					history[calculationId].first = operationProtocol.sessionId;
					history[calculationId].second.push_back(operationProtocol);

					//Dodawanie
					if (operationProtocol.operation == OP_ADD) { calculation(&add, operationProtocol.sessionId); }
					//Odejmowanie
					else if (operationProtocol.operation == OP_SUBT) { calculation(&subtract, operationProtocol.sessionId); }
					//Mno¿enie
					else if (operationProtocol.operation == OP_MULTP) { calculation(&multiply, operationProtocol.sessionId); }
					//Dzielenie
					else if (operationProtocol.operation == OP_DIV) { calculation(&divide, operationProtocol.sessionId); }
					//Silnia
					else if (operationProtocol.operation == OP_FACT) { calculation(&factorial, operationProtocol.sessionId); }

					//Zwiêkszenie sessionId obliczeñ
					calculationId++;
				}
			}
		}
	}

};
