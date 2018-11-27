#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"
#include <random>
#include <ctime>

inline int randInt(const int &min, const int &max) {
	if (max <= min) return min;
	std::random_device rd;
	std::mt19937 gen(rd());
	const std::uniform_int_distribution<int > d(min, max);
	return d(gen);
}
int silnia(int c){
	int temp=1;
	for (int i = 1; i <= c; i++) {
		temp =temp* i;


	}
	return temp;

}

class ServerUDP : public NodeUDP {
public:
	ServerUDP(const std::string& IP, const unsigned short& Port1, const unsigned short& Port2) : NodeUDP(IP, Port1, Port2) {};
	virtual ~ServerUDP() { WSACleanup(); };

	bool start_session() {
		//Czekanie na ¿¹danie sesji
		std::string received;
		receive_text_protocol(received);

		//Wys³anie ID
		 TextProtocol d('p', 0, randInt(10, 99), GET_CURRENT_TIME());

		if (!send_text_protocol(d, 0)) {
			std::cout << "B³¹d wysy³ania.\n";
			return false;
		}
		while (true) {
			if (!session(d)) { break; }
		}



		return true;
	}
	bool session(TextProtocol& d){
		std::string received;
		receive_text_protocol(received);
		d.from_string(received);
		if (d.ST=='r') {//sprawdza czy klient chce sie rozl¹czyæ
			return false;
		
		
		
		}
		if (d.ST == 's') {//sprawdza czy klient chce sie obliczyæ silnie
			
			receive_text_protocol(received);
			d.from_string(received);
			std::cout << d.ST << std::endl;
			if(d.number1>9){
				d.ST = 'e';
				if (!send_text_protocol(d, 0)) {
			
					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
			
				return true;
			}
			else{
				d.number1 = silnia(d.number1);
				d.ST = 's';
				if (!send_text_protocol(d, 0)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				if (!send_text_protocol(d, 2)) {

					std::cout << "B³¹d wysy³ania.\n";
					return false;
				}
				return true;
			
			
			}


			return true;
		}

	
		return true;
	}



};
