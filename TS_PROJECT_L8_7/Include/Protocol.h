#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>


inline std::ostream& operator << (std::ostream& os, const tm& time) {
	os << std::setfill('0') << std::setw(2) << time.tm_hour << ':' << std::setfill('0') << std::setw(2) << time.tm_min << ':' << std::setfill('0') << std::setw(2) << time.tm_sec;
	return os;
}

inline const tm GET_CURRENT_TIME() {
	time_t tt;
	time(&tt);
	tm time;
	localtime_s(&time, &tt);
	time.tm_year += 1900;
	time.tm_mon += 1;
	return time;
}


//HEAD od header (nag³ówek)
//Makra do dodawania w funkcji to_string()
#define HEAD_OP		std::string("Operacja: ")//10
#define HEAD_ST		std::string("Status: ") //8
#define HEAD_NUM_1  std::string("Argument 1: ")//12
#define HEAD_NUM_2  std::string("Argument 2: ")//12
#define HEAD_SN     std::string("Numer Sekwencyjny: ")
#define HEAD_ID     std::string("Identyfikator: ")
#define HEAD_OP_ID  std::string("Identyfikator operacji: ")
#define HEAD_TIME   std::string("Czas: ")
#define HEAD_LENGTH std::string("Dlugoœæ nastêpnego komunikatu: ")

/*
 * Znaczenia poszczególnych bitów w bitset<>:
	* 0 - pole OP
	* 1 - pole ST
	* 2 - pole number1
	* 3 - pole number2
	* 4 - pole OP_ID
 */

class TextProtocol {
public:

	char OP, ST;          //pole operacji, pole statusu
	int number1, number2; //watroœæ 1, wartoœæ 2
	int SN, ID, OP_ID;    //numer sekwencyjny, identyfikator, identyfikator obliczne
	tm  time;   //czas //dlugoœæ

	//Konstruktor domyœlny
	TextProtocol() : OP(0), ST(0), number1(0), number2(0), SN(0), ID(0), OP_ID(0), time() {};

	//Konstruktor przyjmuj¹cy wszystkie pola
	TextProtocol(const char& OP_, const char& ST_, const int& number1_, const int& number2_,
		const int& NS_, const int& ID_, const int& OP_ID_, const tm& time_)
		: OP(OP_), ST(ST_), number1(number1_), number2(number2_), SN(NS_), ID(ID_),
		OP_ID(OP_ID_), time(time_) {}

	//Konstruktor przyjmuj¹cy pola ID i time
	TextProtocol(const char& ST_, const int& SN_, const int& ID_, const tm& time_)
		: TextProtocol(NULL, ST_, NULL, NULL, SN_, ID_, NULL, time_) {}

	//Konstruktor przyhmyj¹cy pola ST, number1, number2, SN, ID, time
	TextProtocol(const char& ST_, const int& number1_, const int& number2_, const int& SN_,
		const int& ID_, const tm& time_)
		: TextProtocol(NULL, ST_, number1_, number2_, SN_, ID_, NULL, time_) {}


	virtual ~TextProtocol() = default;;

	//Serializacja
	std::string to_string(const int& field) const {
		std::string result;

		std::stringstream resultStream;
		result += HEAD_TIME;
		resultStream << time;
		std::string temp;
		resultStream >> temp;
		result += temp + ' ';

		if (field == 1) { std::string temp(1, OP); result += HEAD_OP + temp; }
		else if (field == 0) { std::string temp(1, ST); result += HEAD_ST; result.append(temp); }
		else if (field == 2) { result += HEAD_NUM_1 + std::to_string(number1); }
		else if (field == 3) { result += HEAD_NUM_2 + std::to_string(number2); }
		else if (field == 4) { result += HEAD_OP_ID + std::to_string(OP_ID); }

		result += ' ';
		result += HEAD_SN + std::to_string(SN) + ' ';

		result += HEAD_ID;
		if (ID < 10) { result += '0'; }
		result += std::to_string(ID);

		std::cout << "Result length: " << result.length() << '\n';
		std::cout << "Result: " << result << '\n';
		return result;
	}

	void from_string(const std::string& data) {
		std::cout << "\nfrom_string\n" << "Dane: " << data << '\n';

		//Czas
		auto iterator = data.find(HEAD_TIME);

		std::string temp;
		//Godziny
		for (auto i = iterator + HEAD_TIME.length(); i <= iterator + HEAD_TIME.length() + 1; i++) {
			temp += data[i];
		}
		time.tm_hour = std::stoi(temp);
		//std::cout << "Godzina: " << temp << '\n';
		temp.clear();

		//Minuty
		for (auto i = iterator + HEAD_TIME.length() + 3; i <= iterator + HEAD_TIME.length() + 4; i++) {
			temp += data[i];
		}
		time.tm_min = std::stoi(temp);
		//std::cout << "Minuty: " << temp << '\n';
		temp.clear();

		//Sekundy
		for (auto i = iterator + HEAD_TIME.length() + 6; i <= iterator + HEAD_TIME.length() + 7; i++) {
			temp += data[i];
		}
		//std::cout << "Sekundy: " << temp << '\n';
		time.tm_sec = std::stoi(temp);
		temp.clear();

		//Operacja
		
		iterator = data.find(HEAD_ST);
		if (iterator != std::string::npos) {
			temp = data[iterator + HEAD_ST.length()];
			ST = temp[0];
			temp.clear();
			std::cout << "Status: " << ST << '\n';
		}
		iterator = data.find(HEAD_NUM_1);
		if (iterator != std::string::npos) {
			auto iterator2= data.find(HEAD_SN);

			for (auto i = iterator + HEAD_NUM_1.length(); i <= iterator + HEAD_NUM_1.length() + 1; i++) {
				temp += data[i];
			}
			number1 = stoi(temp);
			temp.clear();
			std::cout << "wynik operacji: " << number1 << '\n';
		}
		
		//Numer Sekwencyjny
		
		iterator = data.find(HEAD_SN);
		//std::cout <<"iterator "<<iterator << '/n';

		for (auto i = iterator + HEAD_SN.length(); i <= iterator + HEAD_SN.length() + 1; i++) {
			temp += data[i];
		}
		SN = std::stoi(temp);
		temp.clear();
		std::cout << "Numer sekwencyjny: " << SN << '\n';
		//Numer Identyfikacyjny
		iterator = data.find(HEAD_ID);
		for (auto i = iterator + HEAD_ID.length(); i <= iterator + HEAD_ID.length() + 1; i++) {
			temp += data[i];
		}
		ID = std::stoi(temp);
		std::cout << "Identyfikator sesji: " << ID << '\n';
	}
};
/*
 * jako ze 4 maks pola to status bedzie informowa³ czy w nastepnym komunikacjie bedzie 1,2
 * argument czy to nawiazywanie  czy konczenie po³¹czenia a ns który to komunikat
 * naziazywanie po³aczenie bedzie wygl¹da³o tak Time|ST|SN|ID gdzie identyfikator to 0 na pocz¹tek a SN
 * liczymy ile jescze pakitów powinno sie wys³aæ(czyli przy nawiazywaniu 0 mamy);()time ma rozmiar 4
 * czyli razem komunikat nawiazywania polaczenia ma rozmiar 13
*/


/*
 * Znaczenia ST:
 * 'p' - po³¹czenie,
 * 'o' - informacja o operacji,
 * 'a' - argumenty,
 * 'r' - roz³¹czenie,
 * 'n' - nadawanie identyfikatora
 */