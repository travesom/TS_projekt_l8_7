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


//HEAD od header (nag��wek)
//Makra do dodawania w funkcji to_string()
#define HEAD_OP "Operacja: "
#define HEAD_ST "Status: "
#define HEAD_NUM_1 "Argument 1: "
#define HEAD_NUM_2 "Argument 2: "
#define HEAD_SN "Numer Sekwencyjny: "
#define HEAD_ID "Identyfikator: "
#define HEAD_OP_ID "Identyfikator operacji: "
#define HEAD_TIME "Czas: "
#define HEAD_LENGTH "Dlugosc nastepnego komunikatu: "

/*
 * Znaczenia poszczeg�lnych bit�w w bitset<>:
	* 0 - pole OP
	* 1 - pole ST
	* 2 - pole number1
	* 3 - pole number2
	* 4 - pole OP_ID
 */

class TextProtocol {
public:

	char OP, ST;          //pole operacji, pole statusu
	int number1, number2; //watro�� 1, warto�� 2
	int SN, ID, OP_ID;    //numer sekwencyjny, identyfikator, identyfikator obliczne
	tm  time;   //czas
	int Length; //dlugo��

	//Konstruktor domy�lny
	TextProtocol() : OP(0), ST(0), number1(0), number2(0), SN(0), ID(0), OP_ID(0), time(), Length(0) {};

	//Konstruktor przyjmuj�cy wszystkie pola
	TextProtocol(const char& OP_, const char& ST_, const int& number1_, const int& number2_,
		const int& NS_, const int& ID_, const int& OP_ID_, const tm& time_)
		: OP(OP_), ST(ST_), number1(number1_), number2(number2_), SN(NS_), ID(ID_),
		OP_ID(OP_ID_), time(time_), Length(0) {}

	//Konstruktor przyjmuj�cy pola ID i time
	TextProtocol(const char& ST_, const int& SN_, const int& ID_, const tm& time_)
		: TextProtocol(NULL, ST_, NULL, NULL, SN_, ID_, NULL, time_) {}

	//Konstruktor przyhmyj�cy pola ST, number1, number2, SN, ID, time
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
		resultStream >> result;
		result += ' ';
		std::cout << "Time result: " << result << '\n';

		if (field == 0) { result += HEAD_OP + std::to_string(OP); }
		else if (field == 1) { result += HEAD_ST + std::to_string(ST); }
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
		//Czas
		auto iterator = data.find(HEAD_TIME);
		std::string temp;
		//Godziny
		for (auto i = iterator + 1; i <= iterator + 3; i++) {
			temp += data[i];
		}
		time.tm_hour = std::stoi(temp);
		temp.clear();

		//Minuty
		for (auto i = iterator + 4; i <= iterator + 6; i++) {
			temp += data[i];
		}
		time.tm_min = std::stoi(temp);
		temp.clear();

		//Sekundy
		for (auto i = iterator + 7; i <= iterator + 9; i++) {
			temp += data[i];
		}
		time.tm_sec = std::stoi(temp);
		temp.clear();

		//Operacja
		iterator = data.find(HEAD_OP);
		temp = data[iterator + 1];
		OP = temp[0];
		temp.clear();

		//Numer Sekwencyjny
		iterator = data.find(HEAD_SN);
		for (auto i = iterator + 1; i <= iterator + 9; i++) {
			temp += data[i];
		}
		SN = std::stoi(temp);
	}
};
/*
 * jako ze 4 maks pola to status bedzie informowa� czy w nastepnym komunikacjie bedzie 1,2
 * argument czy to nawiazywanie  czy konczenie po��czenia a ns kt�ry to komunikat
 * naziazywanie po�aczenie bedzie wygl�da�o tak Time|ST|SN|ID gdzie identyfikator to 0 na pocz�tek a SN
 * liczymy ile jescze pakit�w powinno sie wys�a�(czyli przy nawiazywaniu 0 mamy);()time ma rozmiar 4
 * czyli razem komunikat nawiazywania polaczenia ma rozmiar 13
*/


/*
 * Znaczenia ST:
 * 'p' - po��czenie,
 * 'o' - informacja o operacji,
 * 'a' - argumenty,
 * 'r' - roz��czenie,
 * 'n' - nadawanie identyfikatora
 */