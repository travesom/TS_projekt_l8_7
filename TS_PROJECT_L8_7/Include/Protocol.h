#pragma once
#include <string>
#include <iostream>

//HEAD od header (nag³ówek)
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
	long int  time;       //czas
	int Length;//dlugosc
	//Konstruktor domyœlny
	TextProtocol() : OP(0), ST(0), number1(0), number2(0), SN(0), ID(0), OP_ID(0), time(0), Length(0) {};

	//Konstruktor przyjmuj¹cy wszystkie pola
	TextProtocol(const char& OP_, const char& ST_, const int& number1_, const int& number2_,
		const int& NS_, const int& ID_, const int& OP_ID_, const long int& time_)
		: OP(OP_), ST(ST_), number1(number1_), number2(number2_), SN(NS_), ID(ID_),
		OP_ID(OP_ID_), time(time_), Length(0) {}

	//Konstruktor przyjmuj¹cy pola ID i time
	TextProtocol(const char& ST_, const int& SN_, const int& ID_, const long int& time_)
		: TextProtocol(NULL, ST_, NULL, NULL, SN_, ID_, NULL, time_) {}

	//Konstruktor przyhmyj¹cy pola ST, number1, number2, SN, ID, time
	TextProtocol(const char& ST_, const int& number1_, const int& number2_, const int& SN_,
		const int& ID_, const long int& time_)
		: TextProtocol(NULL, ST_, number1_, number2_, SN_, ID_, NULL, time_) {}


	virtual ~TextProtocol() = default;;

	//Serializacja
	std::string to_string(const int& field) const {
		std::string result;

		result += HEAD_TIME + std::to_string(time) + ' ';
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