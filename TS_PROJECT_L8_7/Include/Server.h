#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Node.h"
#include <random>
#include <ctime>
#include <unordered_map>


int numer_operacji = 0;

class history
{	public:
	int wynik,arg1,arg2,numer_op;
	char operacja[2];//od-odejmowani do-dodawanie mn-mnozenei su-sub(dzielenie) si-silnia
	history(int a, int b, int c, char d[2],int numer_op) {
		wynik = a; arg1 = b; arg2 = c; operacja[0] = d[0]; operacja[1] = d[1];;
		this->numer_op = numer_op;

	}
	history(int a, int b,  char d[2],int numer_op) {
		wynik = a; arg1 = b; operacja[0] = d[0]; operacja[1] = d[1];;
		numer_op = numer_op;

	}
	void asign(int &a, int &b, std::string &d ) {
		a = wynik; b = arg1; d=operacja;
		

	}
	void asign(int &a, int &b, int &c, std::string &d) {
		a = wynik; b = arg1; d = operacja; c = arg2;


	}



};
typedef std::unordered_map<int, std::vector<history>> history_map;
std::vector<history> hist_vector;
history_map historia;
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
		hist_vector.clear();
		historia[d.ID] = hist_vector;
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
				char char_temp [2]{ 's','i' };
				numer_operacji++;
				int arg1=d.number1;
				d.number1 = silnia(d.number1);
				historia[d.ID].push_back(history(d.number1, arg1, char_temp, numer_operacji));
				
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
		if (d.ST == 'o') {//sprawdza czy klient chce sie obliczyæ z 2 argumentami

			receive_text_protocol(received);
			d.from_string(received);
			receive_text_protocol(received);
			d.from_string(received);
			receive_text_protocol(received);
			d.from_string(received);
			d.SN = 1;
			std::cout << d.OP << std::endl;
			std::cout << d.number1 << std::endl;
			std::cout << d.number2 << std::endl;
			
			if(d.OP='d') {
				numer_operacji++;
				char char_temp[2]{ 'd','o' };
				int arg1 = d.number1;
				d.number1 = silnia(d.number1);
				historia[d.ID].push_back(history(d.number1 + d.number2, arg1, d.number2, char_temp, numer_operacji));
				
				
				d.number1 = d.number1 + d.number2;
				d.ST = 'o';
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
			else if (d.OP = 'o') {
				char char_temp[2]{ 'o','d' };
				numer_operacji++;
				int arg1 = d.number1;
				d.number1 = silnia(d.number1);
				historia[d.ID].push_back(history(d.number1 - d.number2, arg1, d.number2, char_temp, numer_operacji));
				
				d.number1 = d.number1 - d.number2;
				d.ST = 'o';
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
			else if (d.OP = 'm') {
				numer_operacji++;
				char char_temp[2]{ 'm','n' };
				numer_operacji++;
				int arg1 = d.number1;
				d.number1 = silnia(d.number1);
				historia[d.ID].push_back(history(d.number1 * d.number2, arg1, d.number2, char_temp, numer_operacji));
				d.number1 = d.number1 * d.number2;
				d.ST = 'o';
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
			else	if (d.OP = 's') {
				numer_operacji++;
				char char_temp[2]{ 's','u' };
				numer_operacji++;
				int arg1 = d.number1;
				d.number1 = silnia(d.number1);
				historia[d.ID].push_back(history(d.number1 / d.number2, arg1, d.number2, char_temp, numer_operacji));
				d.number1 = d.number1 / d.number2;
				d.ST = 'o';
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
		//	else tu jak blad


			return true;
		}
		if (d.ST == 'h') {//sprawdza czy klient chce sie obliczyæ silnie
			int num1, num2,num3;
			char k='c';
			std::string typ;
			receive_text_protocol(received);
			d.from_string(received);
			historia.at(d.ID).at(d.OP_ID).asign(num1,num2,num3,typ);
			d.number1 = num1;
			d.number2 = num2;
			if (typ == "do") { k = 'd'; }
			else if (typ == "od") { k = 'o'; }
			else if (typ == "si") { k = 's'; }
			else if (typ == "su") { k = 'i'; }
			else if (typ == "mn") { k = 'm'; }//dzielnie z nazwa na i
			d.OP = k;
			d.SN = 3;
			if (!send_text_protocol(d, 2)) {

				std::cout << "B³¹d wysy³ania.\n";
				return false;
			}
			d.number1 = num3;
			d.SN--;
			if (!send_text_protocol(d, 2)) {

				std::cout << "B³¹d wysy³ania.\n";
				return false;
			}
			d.SN--;
			if (!send_text_protocol(d, 3)) {

				std::cout << "B³¹d wysy³ania.\n";
				return false;
			}
			d.SN--;
			if (!send_text_protocol(d, 1)) {

				std::cout << "B³¹d wysy³ania.\n";
				return false;
			}

			return true;
		}

	
		return true;
	}



};
