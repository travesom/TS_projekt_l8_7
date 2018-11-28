#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"
#include <random>
#include <ctime>
#include <unordered_map>


int numer_operacji = 0;

inline int randInt(const int &min, const int &max) {
	if (max <= min) return min;
	std::random_device rd;
	std::mt19937 gen(rd());
	const std::uniform_int_distribution<int > d(min, max);
	return d(gen);
}
int silnia(int c) {
	int temp = 1;
	for (int i = 1; i <= c; i++) {
		temp = temp * i;


	}
	return temp;

}

class ServerUDP : public NodeUDP {
public:
	unsigned int currentSessionId;
	unsigned int operationId = 0;

	//Mapa przechowuj¹ca historie sesji
	//Kluczem jest identyfikator sesji, a wartoœci¹ tablica odebranych i wys³anych protoko³ów
	std::unordered_map<unsigned int, std::vector<TextProtocol>> history;
	
	//Pomocnicza mapa ostatnich id operacji dla danego id sesji
	std::unordered_map<unsigned int, unsigned int> lastOperationIds;

	ServerUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) : NodeUDP(IP, Port1, Port2) {};
	virtual ~ServerUDP() { WSACleanup(); };

	bool start_session() {
		//Czekanie na ¿¹danie sesji
		std::string received;
		receive_text_protocol(received);
		TextProtocol receivedProt;
		receivedProt.from_string(received);

		//Wys³anie ID
		unsigned int sessionId = receivedProt.ID;
		//Jeœli klient nie ma identyfikatora sesji
		if (sessionId == 0) {
			while (true) {
				sessionId = randInt(1, 99);
				if (history.find(sessionId) == history.end()) { break; }
			}
		}
		else { operationId = lastOperationIds[sessionId] + 1; }
		history[sessionId].push_back(receivedProt);
		currentSessionId = sessionId;

		TextProtocol protocol("Identyfikator", 0, sessionId, GET_CURRENT_TIME());
		history[sessionId].push_back(protocol);

		if (!send_text_protocol(protocol, 0)) {
			std::cout << "B³¹d wysy³ania.\n";
			return false;
		}
		while (true) {
			if (!session()) { break; }
		}

		currentSessionId = 0;
		return true;
	}

	bool session() {
		TextProtocol protocol;
		std::string received;

		receive_text_protocol(received);
		protocol.from_string(received);
		history[protocol.ID].push_back(protocol); //Dodanie komunikatu do historii

		if (protocol.ST == "Zakonczenie") {//sprawdza czy klient chce sie rozl¹czyæ
			return false;
		}
		if (protocol.ST == "Operacja") {//Sprawdza czy klient chce wykonaæ operacjê
			if(protocol.OP == "Silnia"){//Typ operacji silnia
				receive_text_protocol(received); //Odebranie komunikatu z numerem sekwencyjnym
				protocol.from_string(received);
				history[protocol.ID].push_back(protocol); //Dodanie komunikatu do historii
				std::cout << protocol.SN << std::endl;

				//Wys³anie identyfikatora dla danych obliczeñ klientowi
				const TextProtocol sendProtocol("Identyfikator_Obliczen",currentSessionId,operationId,GET_CURRENT_TIME());
				operationId++;
				send_text_protocol(sendProtocol, 3);

				//Odbieranie tyle razy, na ile wskazuje numer sekwencyjny
				for(unsigned int i = 0; i < protocol.SN;i++){
					receive_text_protocol(received);
					protocol.from_string(received);
					history[protocol.ID].push_back(protocol); //Dodanie komunikatu do historii
				}

				if (protocol.number > 9) {
					protocol.ST = 'e';
					if (!send_text_protocol(protocol, 0)) {

						std::cout << "B³¹d wysy³ania.\n";
						return false;
					}

					return true;
				}
				else {
					char char_temp[2]{ 's','i' };
					numer_operacji++;
					int arg1 = protocol.number;
					protocol.number = silnia(protocol.number);
					history[protocol.ID].push_back(protocol);

					protocol.ST = 's';
					if (!send_text_protocol(protocol, 0)) {

						std::cout << "B³¹d wysy³ania.\n";
						return false;
					}
					if (!send_text_protocol(protocol, 2)) {

						std::cout << "B³¹d wysy³ania.\n";
						return false;
					}
					return true;
				}
				return true;
			}
		}
		//Rzeczy st¹d powinny pójœæ do if'a wy¿ej i do oddzielnych if'ów obok silni
		if (protocol.ST == "Operacja") {//sprawdza czy klient chce sie obliczyæ z 2 argumentami

			receive_text_protocol(received);
			protocol.from_string(received);
			history[protocol.ID].push_back(protocol);
			receive_text_protocol(received);
			protocol.from_string(received);
			history[protocol.ID].push_back(protocol);
			receive_text_protocol(received);
			protocol.from_string(received);
			history[protocol.ID].push_back(protocol);

			protocol.SN = 1;
			std::cout << protocol.OP << std::endl;
			std::cout << protocol.number << std::endl;

			if (protocol.OP == "protocol") {
				numer_operacji++;
				char char_temp[2]{ 'd','o' };
				int arg1 = protocol.number;

				protocol.number = silnia(protocol.number);


				protocol.number = protocol.number;
				protocol.ST = 'o';
				if (!send_text_protocol(protocol, 0)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				if (!send_text_protocol(protocol, 2)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				return true;


			}
			else if (protocol.OP == "o") {
				char char_temp[2]{ 'o','d' };
				numer_operacji++;
				int arg1 = protocol.number;
				protocol.number = silnia(protocol.number);
				//historia[protocol.ID].push_back(history(protocol.number - protocol.number2, arg1, protocol.number2, char_temp, numer_operacji));

				//protocol.number = protocol.number - protocol.number2;
				protocol.ST = 'o';
				if (!send_text_protocol(protocol, 0)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				if (!send_text_protocol(protocol, 2)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				return true;


			}
			else if (protocol.OP == "m") {
				numer_operacji++;
				char char_temp[2]{ 'm','n' };
				numer_operacji++;
				int arg1 = protocol.number;
				protocol.number = silnia(protocol.number);
				//historia[protocol.ID].push_back(history(protocol.number * protocol.number2, arg1, protocol.number2, char_temp, numer_operacji));
				//protocol.number = protocol.number * protocol.number2;
				protocol.ST = 'o';
				if (!send_text_protocol(protocol, 0)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				if (!send_text_protocol(protocol, 2)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				return true;


			}
			else	if (protocol.OP == "s") {
				numer_operacji++;
				char char_temp[2]{ 's','u' };
				numer_operacji++;
				int arg1 = protocol.number;
				protocol.number = silnia(protocol.number);
				//historia[protocol.ID].push_back(history(protocol.number / protocol.number2, arg1, protocol.number2, char_temp, numer_operacji));
				//protocol.number = protocol.number / protocol.number2;
				protocol.ST = 'o';
				if (!send_text_protocol(protocol, 0)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				if (!send_text_protocol(protocol, 2)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				return true;


			}
			//	else tu jak blad


			return true;
		}
		if (protocol.ST == "Historia") {//Sprawdza czy klient uzyskaæ historiê obliczeñ
			//Wys³anie klientowi numer sekwencyjny (iloœæ wpisów w historii)
			TextProtocol sendProt("Numer_Sekwencyjny", history[currentSessionId].size(), currentSessionId, GET_CURRENT_TIME());
			send_text_protocol(sendProt,0);
		}


		return true;
	}



};
