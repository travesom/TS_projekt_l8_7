#pragma once
#include "Defines.h"
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


inline void double_remove_end_zero(std::string& numberStr) {
	if (numberStr.find('.') != std::string::npos) {
		for (unsigned int i = numberStr.size() - 1; i > 0; i--) {
			if (numberStr[i] == '0') {
				numberStr.pop_back();
			}
			else if (numberStr[i] == '.') {
				numberStr.pop_back();
				break;
			}
			else { break; }
		}
	}
}

class TextProtocol {
public:
	tm  time;   //znacznik czasowy
	unsigned int id;             //Identyfikator sesji
	unsigned int sequenceNumber; //Numer sekwencyjny (pole obowi�zkowe)

	/**
	* Znaczenia operation: \n
	* OP_BEGIN - rozpocz�cie sesji, \n
	*
	* OP_ADD - informacja o obliczeniach, \n
	* OP_SUBT - informacja o obliczeniach, \n
	* OP_DIV - informacja o obliczeniach, \n
	* OP_MULTP - informacja o obliczeniach, \n
	* OP_FACT - informacja o obliczeniach, \n
	* \n
	* OP_ARGUMENT - wys�anie argumentu,\n
	* OP_RESULT - wys�anie wyniku, \n
	* \n
	* OP_HISTORY_WHOLE \n
	* "HISTORIA_ID" \n
	* \n
	* OP_STATUS - wys�anie statusu operacji \n
	* \n
	* OP_ID_SESSION - nadawanie identyfikatora sesji \n
	* OP_ID_CALCULATION - przesy�anie identyfikatora oblicze� \n
	* \n
	* OP_END - roz��czenie, \n
	*/
	std::string operation;    //Pole operacji

	/**
	* Mo�liwe statusy: \n
	* "WYNIK_POZA_ZAKRESEM" \n
	* "OPERACJA_UDANA" \n
	*/
	std::string status;          //Pole statusu
	double number;               //Pole liczby
	unsigned int calculationId;  //Identyfikator oblicze�

	//Konstruktor domy�lny
	TextProtocol() : time(), id(NULL), sequenceNumber(NULL), operation(""),status(""),number(NAN),calculationId(NULL) {};

	TextProtocol(const tm& time_, const unsigned int& id_, const unsigned int& sequenceNumber_) : TextProtocol() {
		time = time_;
		id = id_;
		sequenceNumber = sequenceNumber_;
	}

	explicit TextProtocol(const std::string& data) { from_string(data); }

	virtual ~TextProtocol() = default;;

	//Serializacja
	/**
	* Znaczenia poszczeg�lnych warto�ci int field: \n
	* -1 - brak p�l nieobowi�zkowych \n
	*  0 - pole sequencetial number \n
	*  1 - pole number \n
	*  2 - pole calculationId \n
	*  3 - pole statusu \n
	*/
	std::string to_string(const int& field) const {
		std::string result;

		//Dodanie znacznika czasowego (pole obowi�zkowe)
		std::stringstream resultStream;
		result += HEAD_TIME;
		resultStream << time;
		std::string temp;
		resultStream >> temp;
		result += temp + ' ';

		//Dodanie identyfikatora sesji (pole obowi�zkowe)
		result += HEAD_SESSION_ID + std::to_string(id) + ' ';

		//Pole numer sekwencyjny (pole obowi�zkowe)
		result += HEAD_SEQNUM; result += std::to_string(sequenceNumber) + ' ';

		//Pole numer operacja
		if (field == FIELD_OPERATION) { result += HEAD_OPERATION + operation; }
		//Pole liczba
		else if (field == FIELD_NUMBER) {
			std::string numberStr = std::to_string(number);
			double_remove_end_zero(numberStr);
			result += HEAD_NUMBER + numberStr;
		}
		//Pole id oblicze�
		else if (field == FIELD_CALCULATION_ID) { result += HEAD_CALC_ID + std::to_string(calculationId); }
		//Pole status
		else if (field == FIELD_STATUS) { result += HEAD_STATUS + status; }

		result += ' ';

		return result;
	}
	unsigned int get_field() const {
		if (!operation.empty())  { return FIELD_OPERATION; }
		if (calculationId != 0) { return FIELD_CALCULATION_ID; }
		if (!status.empty()) { return FIELD_STATUS; }
		if (!isnan(number)) { return FIELD_NUMBER; }
		return FIELD_NO_ADDITIONAL;
	}

	//Deserializacja
	void from_string(const std::string& data) {
		this->clear();
		auto iterator = data.find(HEAD_TIME);
		std::string temp;


		//Czas (pole obowi�zkowe)
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

		//Numer Identyfikacyjny (pole obowi�zkowe)
		{
			temp.clear();
			iterator = data.find(HEAD_SESSION_ID);
			for (auto i = iterator + HEAD_SESSION_ID.length(); i < data.size(); i++) {
				if (data[i] == '\0' || data[i] == ' ') { break; }
				temp += data[i];
			}
			temp += ' ';
			id = std::stoi(temp);
		}

		//OPERACJA (pole obowi�zkowe)
		{
			iterator = data.find(HEAD_OPERATION);
			if (iterator != std::string::npos) {
				temp.clear();
				for (auto i = iterator + HEAD_OPERATION.length(); i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == ' ') { break; }
					temp += data[i];
				}
				operation = temp;
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
			}
		}

		//Identyfikator oblicze�
		{
			iterator = data.find(HEAD_CALC_ID);
			if (iterator != std::string::npos) {
				temp.clear();
				for (auto i = iterator + HEAD_CALC_ID.length(); i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == ' ') { break; }
					temp += data[i];
				}
				calculationId = stoi(temp);
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

	//Zerowanie protoko�u
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
 * jako ze 4 maks pola to status bedzie informowa� czy w nastepnym komunikacjie bedzie 1,2
 * argument czy to nawiazywanie  czy konczenie po��czenia a ns kt�ry to komunikat
 * naziazywanie po�aczenie bedzie wygl�da�o tak Time|status|sequenceNumber|id gdzie identyfikator to 0 na pocz�tek a sequenceNumber
 * liczymy ile jescze pakit�w powinno sie wys�a�(czyli przy nawiazywaniu 0 mamy);()time ma rozmiar 4
 * czyli razem komunikat nawiazywania polaczenia ma rozmiar 13
*/