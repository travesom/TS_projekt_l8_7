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
#define HEAD_NUM    std::string("Argument: ")//12
#define HEAD_SN     std::string("Numer Sekwencyjny: ")
#define HEAD_ID     std::string("Identyfikator: ")
#define HEAD_OP_ID  std::string("Identyfikator operacji: ")
#define HEAD_TIME   std::string("Czas: ")


class TextProtocol {
public:
	/**
	 * Mo¿liwoœci pola operacji:
	 * "Dodawanie"
	 * "Odejmowanie"
	 * "Mno¿enie"
	 * "Dzielenie"
	 * "Silnia"
	 */
	std::string OP;    //Pole operacji

	/**
	* Znaczenia ST: \n
	* "Rozpoczecie" - rozpoczêcie sesji, \n
	* "Operacja" - informacja o operacji, \n
	* "Argument" - argumenty, \n
	* "Zakonczenie" - roz³¹czenie, \n
	* "Identyfikator_Sesji" - nadawanie identyfikatora sesji
	* "Identyfikator_Obliczen" - przesy³anie identyfikatora obliczeñ
	* "Numer_Sekwencyjny" - przes³anie numeru sekwencyjnego
	*/
	std::string ST;
	int number; //Pole liczby
	unsigned int SN;     //Numer sekwencyjny
	int ID;     //Identyfikator sesji
	unsigned int OP_ID;  //Identyfikator obliczeñ
	tm  time;   //znacznik czasowy

	//Konstruktor domyœlny
	TextProtocol() : OP(""), ST(""), number(0), SN(0), ID(0), OP_ID(0), time() {};

	//Konstruktor przyjmuj¹cy wszystkie pola
	TextProtocol(const std::string& OP_, const std::string& ST_, const int& number1_,
		const int& NS_, const int& ID_, const int& OP_ID_, const tm& time_)
		: OP(OP_), ST(ST_), number(number1_), SN(NS_), ID(ID_),
		OP_ID(OP_ID_), time(time_) {}

	//Konstruktor przyjmuj¹cy pola ST, SN, ID i time (number tylko po to ¿eby przeci¹¿enia nie by³y takie same)
	TextProtocol(const std::string& ST_, const int& number_, const int& SN_, const int& ID_, const tm& time_)
		: TextProtocol("", ST_, NULL, SN_, ID_, NULL, time_) {}

	//Konstruktor przyjmuj¹cy pola ST, ID, OP_ID, time
	TextProtocol(const std::string& ST_, const int& ID_, const unsigned& OP_ID_, const tm& time_)
		: TextProtocol("", ST_, NULL, NULL, ID_, OP_ID_, time_) {}

	//Konstruktor przyjmuj¹cy pola ST, ID, time
	TextProtocol(const std::string& ST_, const int& ID_, const tm& time_)
		: TextProtocol("", ST_, NULL, NULL, ID_, NULL, time_) {}

	//Konstruktor przyjmuj¹cy pola ST, number, number2, SN, ID, time
	TextProtocol(const std::string& ST_, const int& number_, const int& number2_, const int& SN_,
		const int& ID_, const tm& time_)
		: TextProtocol("", ST_, number_, SN_, ID_, NULL, time_) {}


	virtual ~TextProtocol() = default;;

	//Serializacja
	/**
	* Znaczenia poszczególnych wartoœci int field: \n
	* -1 - brak pól nieobowi¹zkowych
	* 0 - pole SN \n
	* 1 - pole OP \n
	* 2 - pole number \n
	* 3 - pole OP_ID
	*/
	std::string to_string(const int& field) const {
		std::string result;

		std::stringstream resultStream;
		result += HEAD_TIME;
		resultStream << time;
		std::string temp;
		resultStream >> temp;
		result += temp + ' ';

		if (field == 0) { result += HEAD_ST; result += ST; }
		else if (field == 1) { result += HEAD_OP + OP; }
		else if (field == 2) { result += HEAD_NUM + std::to_string(number); }
		else if (field == 3) { result += HEAD_OP_ID + std::to_string(OP_ID); }

		result += ' ';
		result += HEAD_SN + std::to_string(SN) + ' ';

		result += HEAD_ID;
		result += std::to_string(ID);

		//std::cout << "Result length: " << result.length() << '\n';
		//std::cout << "Result: " << result << '\n';
		return result;
	}

	void from_string(const std::string& data) {
		this->clear();
		//std::cout << "\nfrom_string\n" << "Dane: " << data << '\n';
		auto iterator = data.find(HEAD_TIME);
		std::string temp;


		//Czas (pole obowi¹zkowe)
		{
			//Godziny
			for (auto i = iterator + HEAD_TIME.length(); i <= iterator + HEAD_TIME.length() + 1; i++) {
				temp += data[i];
			}
			time.tm_hour = std::stoi(temp);
			temp.clear();

			//Minuty
			for (auto i = iterator + HEAD_TIME.length() + 3; i <= iterator + HEAD_TIME.length() + 4; i++) {
				temp += data[i];
			}
			time.tm_min = std::stoi(temp);
			temp.clear();

			//Sekundy
			for (auto i = iterator + HEAD_TIME.length() + 6; i <= iterator + HEAD_TIME.length() + 7; i++) {
				temp += data[i];
			}
			time.tm_sec = std::stoi(temp);
			temp.clear();
		}

		//Status (pole obowi¹zkowe)
		iterator = data.find(HEAD_ST);
		if (iterator != std::string::npos) {
			temp = data[iterator + HEAD_ST.length()];
			ST = temp[0];
			temp.clear();
			//std::cout << "Status: " << ST << '\n';
		}

		//Numer Identyfikacyjny (pole obowi¹zkowe)
		iterator = data.find(HEAD_ID);
		for (auto i = iterator + HEAD_ID.length(); i < data.size(); i++) {
			if (data[i] == '\0' || data[i] == ' ') { break; }
			temp += data[i];
		}
		temp += ' ';
		ID = std::stoi(temp);

		//Liczba
		iterator = data.find(HEAD_NUM);
		if (iterator != std::string::npos) {
			for (auto i = iterator + HEAD_NUM.length(); i < data.size(); i++) {
				if (data[i] == '\0' || data[i] == ' ') { break; }
				temp += data[i];
			}
			number = stoi(temp);
			temp.clear();
			//std::cout << "wynik operacji: " << number << '\n';
		}

		//Operacja
		iterator = data.find(HEAD_OP);
		if (iterator != std::string::npos) {
			for (auto i = iterator + HEAD_OP.length(); i < data.size(); i++) {
				if (data[i] == '\0' || data[i] == ' ') { break; }
				temp += data[i];
			}
			OP = temp;
			temp.clear();
			//std::cout << "Typ operacji: " << OP << '\n';
		}

		//Identyfikator obliczeñ
		iterator = data.find(HEAD_OP_ID);
		if (iterator != std::string::npos) {
			for (auto i = iterator + HEAD_OP_ID.length(); i < data.size(); i++) {
				if (data[i] == '\0' || data[i] == ' ') { break; }
				temp += data[i];
			}
			OP_ID = stoi(temp);
			temp.clear();
			//std::cout << "wynik operacji: " << OP_ID << '\n';
		}

		//Numer Sekwencyjny
		iterator = data.find(HEAD_SN);

		for (auto i = iterator + HEAD_SN.length(); i < data.size(); i++) {
			if (data[i] == '\0' || data[i] == ' ') { break; }
			temp += data[i];
		}
		SN = std::stoi(temp);
		temp.clear();
	}

	void clear(){
		OP = "";
		ST = "";
		number = NULL;
		SN = NULL;
		ID = NULL;
		OP_ID = NULL;
		time = tm();
	}
};
/*
 * jako ze 4 maks pola to status bedzie informowa³ czy w nastepnym komunikacjie bedzie 1,2
 * argument czy to nawiazywanie  czy konczenie po³¹czenia a ns który to komunikat
 * naziazywanie po³aczenie bedzie wygl¹da³o tak Time|ST|SN|ID gdzie identyfikator to 0 na pocz¹tek a SN
 * liczymy ile jescze pakitów powinno sie wys³aæ(czyli przy nawiazywaniu 0 mamy);()time ma rozmiar 4
 * czyli razem komunikat nawiazywania polaczenia ma rozmiar 13
*/