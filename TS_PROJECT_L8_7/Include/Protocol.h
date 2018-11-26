#pragma once
#include <time.h>
#include <stdio.h>
#include <bitset>

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

/*
 * Znaczenia poszczególnych bitów w bitset<>:
	* 0 - pole OP
	* 1 - pole ST
	* 2 - pole number1
	* 3 - pole number2
	* 4 - pole SN
	* 5 - pole ID
	* 6 - pole OP_ID
	* 7 - pole time
 */
//Makra do decydowania, które pola maj¹ byæ uwzglêdnione w to_string()
#define PROT_ID std::bitset<8>("01001101")
#define PROT_NUMBER_OP std::bitset<8>("01001101")

class TextProtocol {
public:

	char OP, ST;          //pole operacji, pole statusu
	int number1, number2; //watroœæ 1, wartoœæ 2
	int SN, ID, OP_ID;    //numer sekwencyjny, identyfikator, identyfikator obliczne
	long int  time;       //czas

	//Konstruktor domyœlny
	TextProtocol() {};

	//Konstruktor przyjmuj¹cy wszystkie pola
	TextProtocol(const char& OP_, const char& ST_, const int& number1_, const int& number2_,
		const int& NS_, const int& ID_, const int& OP_ID_, const long int& time_)
		: OP(OP_), ST(ST_), number1(number1_), number2(number2_), SN(NS_), ID(ID_),
		OP_ID(OP_ID_), time(time_) {}
	
	//Konstruktor przyjmuj¹cy pola ID i time
	TextProtocol(const char& ST_, const int& SN_, const int& ID_, const long int& time_) 
				 : TextProtocol(NULL, ST_, NULL, NULL, SN_, ID_, NULL, time_){}

	//Konstruktor przyhmyj¹cy pola ST, number1, number2, SN, ID, time
	TextProtocol(const char& ST_, const int& number1_, const int& number2_, const int& SN_,
		const int& ID_, const long int& time_)
		: TextProtocol(NULL, ST_, number1_, number2_, SN_, ID_, NULL, time_) {}


	virtual ~TextProtocol() {};

	//Serializacja
	std::string to_string(const std::bitset<8>& fields) const {
		std::string result;

		int fieldNumber = 0; //Zmienna do œledzenia ile pól zosta³o ju¿ dodanych

		if (fields[0]) { result += HEAD_TIME  + std::to_string(OP) + ' ';      fieldNumber++; }
		if (fields[1]) { result += HEAD_ST    + std::to_string(ST) + ' ';      fieldNumber++; }
		if (fields[2]) { result += HEAD_NUM_1 + std::to_string(number1) + ' '; fieldNumber++; }
		if (fields[3]) { result += HEAD_NUM_2 + std::to_string(number2) + ' '; fieldNumber++; }
		//Zwrócenie wyniku, jeœli ju¿ dodano 4 pola
		if (fieldNumber == 4) { return result; }

		if (fields[4]) { result += HEAD_SN    + std::to_string(SN) + ' ';      fieldNumber++; }
		//Zwrócenie wyniku, jeœli ju¿ dodano 4 pola
		if (fieldNumber == 4) { return result; }

		if (fields[5]) { result += HEAD_ID    + std::to_string(ID) + ' ';      fieldNumber++; }
		//Zwrócenie wyniku, jeœli ju¿ dodano 4 pola
		if (fieldNumber == 4) { return result; }

		if (fields[6]) { result += HEAD_OP_ID + std::to_string(OP_ID) + ' ';   fieldNumber++; }
		//Zwrócenie wyniku, jeœli ju¿ dodano 4 pola
		if (fieldNumber == 4) { return result; }

		if (fields[7]) { result += HEAD_TIME  + std::to_string(time) + ' ';}

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