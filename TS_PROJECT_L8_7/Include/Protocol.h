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
#define HEAD_OPERATION		std::string("OPERACJA: ")//10
#define HEAD_STATUS		std::string("Status: ") //8
#define HEAD_NUMBER    std::string("Liczba: ")//12
#define HEAD_SEQNUM     std::string("Numer Sekwencyjny: ")
#define HEAD_ID     std::string("Identyfikator: ")
#define HEAD_CALC_ID  std::string("Identyfikator operacji: ")
#define HEAD_TIME   std::string("Czas: ")

#define STATUS_CALC_SUCCESS std::string("OPERACJA_UDANA")
#define STATUS_CALC_OUT_OF_RANGE std::string("WYNIK_POZA_ZAKRESEM")

class TextProtocol {
public:
	/**
	* Znaczenia operation: \n
	* "Rozpoczecie" - rozpoczêcie sesji, \n
	*
	* "DODAWANIE" - informacja o obliczeniach, \n
	* "ODEJMOWANIE" - informacja o obliczeniach, \n
	* "DZIELENIE" - informacja o obliczeniach, \n
	* "MNOZENIE" - informacja o obliczeniach, \n
	* "SILNIA" - informacja o obliczeniach, \n
	* \n
	* "ARGUMENT" - wys³anie argumentu,\n
	* "WYNIK" - wys³anie wyniku, \n
	* \n
	* "HISTORIA_CALA" \n
	* "HISTORIA_ID" \n
	* \n
	* "STATUS" - wys³anie statusu operacji \n
	* \n
	* "IDENTYFIKATOR_SESJI" - nadawanie identyfikatora sesji \n
	* "IDENTYFIKATOR_OBLICZEN" - przesy³anie identyfikatora obliczeñ \n
	* "NUMER_SEKWENCYJNY" - przes³anie numeru sekwencyjnego \n
	* \n
	* "ZAKONCZENIE" - roz³¹czenie, \n
	*/
	std::string operation;    //Pole operacji

	/**
	* Mo¿liwe statusy: \n
	* "WYNIK_POZA_ZAKRESEM" \n
	* "OPERACJA_UDANA" \n
	*/
	std::string status;
	double number; //Pole liczby
	unsigned int sequenceNumber;     //Numer sekwencyjny
	int id;     //Identyfikator sesji
	unsigned int calculationId;  //Identyfikator obliczeñ
	tm  time;   //znacznik czasowy

	//Konstruktor domyœlny
	TextProtocol() : operation(""), status(""), number(0), sequenceNumber(0), id(0), calculationId(0), time() {};

	//Konstruktor przyjmuj¹cy wszystkie pola
	TextProtocol(const std::string& OP_, const std::string& ST_, const int& number1_,
		const int& NS_, const int& ID_, const int& OP_ID_, const tm& time_)
		: operation(OP_), status(ST_), number(number1_), sequenceNumber(NS_), id(ID_),
		calculationId(OP_ID_), time(time_) {}

	TextProtocol(const tm& time_, const unsigned int& id_, const std::string& operation_) {
		number = 0;
		time = time_;
		id = id_;
		operation = operation_;
	}

	explicit TextProtocol(const std::string& data) { from_string(data); }

	virtual ~TextProtocol() = default;;

	//Serializacja
	/**
	* Znaczenia poszczególnych wartoœci int field: \n
	* -1 - brak pól nieobowi¹zkowych \n
	*  0 - pole sequencetial number \n
	*  1 - pole number \n
	*  2 - pole calculationId \n
	*  3 - pole statusu \n
	*/
	std::string to_string(const int& field) const {
		std::string result;

		//Dodanie znacznika czasowego (pole obowi¹zkowe)
		std::stringstream resultStream;
		result += HEAD_TIME;
		resultStream << time;
		std::string temp;
		resultStream >> temp;
		result += temp + ' ';

		//Dodanie identyfikatora sesji (pole obowi¹zkowe)
		result += HEAD_ID + std::to_string(id) + ' ';

		//Dodanie operacji (pole obowi¹zkowe)
		result += HEAD_OPERATION + operation + ' ';

		//Pola nieobowi¹zkowe
		if (field == 0) { result += HEAD_SEQNUM; result += std::to_string(sequenceNumber); }
		else if (field == 1) { result += HEAD_NUMBER + std::to_string(number); }
		else if (field == 2) { result += HEAD_CALC_ID + std::to_string(calculationId); }
		else if (field == 3) { result += HEAD_STATUS + status; }

		result += ' ';

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
			temp.clear();
			for (auto i = iterator + HEAD_TIME.length(); i <= iterator + HEAD_TIME.length() + 1; i++) {
				temp += data[i];
			}
			time.tm_hour = std::stoi(temp);

			//Minuty
			temp.clear();
			for (auto i = iterator + HEAD_TIME.length() + 3; i <= iterator + HEAD_TIME.length() + 4; i++) {
				temp += data[i];
			}
			time.tm_min = std::stoi(temp);


			//Sekundy
			temp.clear();
			for (auto i = iterator + HEAD_TIME.length() + 6; i <= iterator + HEAD_TIME.length() + 7; i++) {
				temp += data[i];
			}
			time.tm_sec = std::stoi(temp);
		}

		//Numer Identyfikacyjny (pole obowi¹zkowe)
		{
			temp.clear();
			iterator = data.find(HEAD_ID);
			for (auto i = iterator + HEAD_ID.length(); i < data.size(); i++) {
				if (data[i] == '\0' || data[i] == ' ') { break; }
				temp += data[i];
			}
			temp += ' ';
			id = std::stoi(temp);
		}

		//OPERACJA (pole obowi¹zkowe)
		{
			iterator = data.find(HEAD_OPERATION);
			if (iterator != std::string::npos) {
				temp.clear();
				for (auto i = iterator + HEAD_OPERATION.length(); i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == ' ') { break; }
					temp += data[i];
				}
				operation = temp;
				//std::cout << "Typ operacji: " << operation << '\n';
			}
		}

		//Status 
		{
			iterator = data.find(HEAD_STATUS);
			if (iterator != std::string::npos) {
				temp.clear();
				for (auto i = iterator + HEAD_STATUS.length(); i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == ' ') { break; }
					temp += data[i];
				}
				status = temp;
				//std::cout << "Status: " << status << '\n';
			}
		}

		//Liczba
		{
			iterator = data.find(HEAD_NUMBER);
			if (iterator != std::string::npos) {
				temp.clear();
				for (auto i = iterator + HEAD_NUMBER.length(); i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == ' ') { break; }
					temp += data[i];
				}
				number = stod(temp);
				//std::cout << "wynik operacji: " << number << '\n';
			}
		}

		//Identyfikator obliczeñ
		{
			iterator = data.find(HEAD_CALC_ID);
			if (iterator != std::string::npos) {
				temp.clear();
				for (auto i = iterator + HEAD_CALC_ID.length(); i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == ' ') { break; }
					temp += data[i];
				}
				calculationId = stoi(temp);
				//std::cout << "wynik operacji: " << calculationId << '\n';
			}
		}

		//Numer Sekwencyjny
		{
			iterator = data.find(HEAD_SEQNUM);
			if (iterator != std::string::npos) {
				temp.clear();
				for (auto i = iterator + HEAD_SEQNUM.length(); i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == ' ') { break; }
					temp += data[i];
				}
				sequenceNumber = std::stoi(temp);
			}
		}
	}

	void clear() {
		operation = "";
		status = "";
		number = NULL;
		sequenceNumber = NULL;
		id = NULL;
		calculationId = NULL;
		time = tm();
	}
};
/*
 * jako ze 4 maks pola to status bedzie informowa³ czy w nastepnym komunikacjie bedzie 1,2
 * argument czy to nawiazywanie  czy konczenie po³¹czenia a ns który to komunikat
 * naziazywanie po³aczenie bedzie wygl¹da³o tak Time|status|sequenceNumber|id gdzie identyfikator to 0 na pocz¹tek a sequenceNumber
 * liczymy ile jescze pakitów powinno sie wys³aæ(czyli przy nawiazywaniu 0 mamy);()time ma rozmiar 4
 * czyli razem komunikat nawiazywania polaczenia ma rozmiar 13
*/