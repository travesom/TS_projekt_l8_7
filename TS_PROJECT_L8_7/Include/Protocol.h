#pragma once
#include "Defines.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Console.hpp"

//Zmienna decyduj¹ca czy na koñcu komunikatu jest dodawan spacja
static const bool MESSAGE_END_SPACE = true;

inline std::ostream& operator << (std::ostream& os, const tm& time) {
	os << std::setfill('0') << std::setw(2) << time.tm_mday << '.' << std::setfill('0') << std::setw(2) << time.tm_mon << '.' << std::setfill('0') << std::setw(4) << time.tm_year << '.'
		<< std::setfill('0') << std::setw(2) << time.tm_hour << ':' << std::setfill('0') << std::setw(2) << time.tm_min << ':' << std::setfill('0') << std::setw(2) << time.tm_sec;
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
	//Pola obowi¹zkowe
	tm  time;                    //znacznik czasowy
	unsigned int sessionId;      //Identyfikator sesji
	unsigned int sequenceNumber; //Numer sekwencyjny (pole obowi¹zkowe)

	//Pola wybieralne
	std::string operation;      //Pole operacji
	std::string status;         //Pole statusu
	std::string address;        //Pole adresu (wykorzystywane przy szukaniu serwera)
	double number;              //Pole liczby
	unsigned int calculationId; //Identyfikator obliczeñ

	//Konstruktor domyœlny
	TextProtocol() : time(), sessionId(NULL), sequenceNumber(NULL), operation(""), status(""), number(NAN), calculationId(NULL) {};

	//Konstruktor przyjmuj¹cy wartoœci dla pól obowi¹zkowych
	TextProtocol(const tm& time_, const unsigned int& id_, const unsigned int& sequenceNumber_) : TextProtocol() {
		time = time_;
		sessionId = id_;
		sequenceNumber = sequenceNumber_;
	}

	//Konstruktor przyjmuj¹cy komunikat w formie ci¹gu znaku
	explicit TextProtocol(const std::string& data) { from_string(data); }

	//Destruktor
	virtual ~TextProtocol() = default;

	//Serializacja
	std::string to_string(const int& field) const {
		std::string result;

		//Dodanie znacznika czasowego (pole obowi¹zkowe)
		{
			std::stringstream resultStream;
			result += HEAD_TIME;
			resultStream << time;
			std::string temp;
			resultStream >> temp;
			result += temp + ' ';
		}

		//Dodanie identyfikatora sesji (pole obowi¹zkowe)
		result += HEAD_SESSION_ID + std::to_string(sessionId) + ' ';

		//Pole numer sekwencyjny (pole obowi¹zkowe)
		result += HEAD_SEQ_NUM; result += std::to_string(sequenceNumber) + ' ';

		//Pole numer operacja
		if (field == FIELD_OPERATION) { result += HEAD_OPERATION + operation; }
		//Pole liczba
		else if (field == FIELD_NUMBER) {
			std::string numberStr = std::to_string(number);
			double_remove_end_zero(numberStr);
			result += HEAD_NUMBER + numberStr;
		}
		//Pole sessionId obliczeñ
		else if (field == FIELD_CALCULATION_ID) { result += HEAD_CALC_ID + std::to_string(calculationId); }
		//Pole status
		else if (field == FIELD_STATUS) { result += HEAD_STATUS + status; }
		//Pole status
		else if (field == FIELD_ADDRESS) { result += HEAD_ADDRESS + address; }

		if (MESSAGE_END_SPACE) { result += ' '; }

		return result;
	}
	unsigned int get_field() const {
		if (!operation.empty()) { return FIELD_OPERATION; }
		if (!address.empty()) { return FIELD_ADDRESS; }
		if (!status.empty()) { return FIELD_STATUS; }
		if (calculationId != 0) { return FIELD_CALCULATION_ID; }
		if (!isnan(number)) { return FIELD_NUMBER; }
		return FIELD_NO_ADDITIONAL;
	}

	//Deserializacja
	void from_string(const std::string& data) {
		this->clear();
		if (!data.empty()) {
			std::string temp;
			auto read_until = [&data](const unsigned int& from, const char& sign, std::string& temp) {
				temp.clear();
				for (auto i = from; i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == sign) { break; }
					temp += data[i];
				}
			};
			auto parse_header = [&read_until, &data](const std::string& header, std::string& temp) {
				const auto iterator = data.find(header);
				if (iterator != std::string::npos) {
					read_until(iterator + header.length(), ' ', temp);
					return true;
				}
				else { return false; }
			};

			//Czas (pole obowi¹zkowe)
			{
				const auto iterator = data.find(HEAD_TIME);

				//Godziny
				read_until(iterator + HEAD_TIME.length(), '.', temp);
				time.tm_mday = std::stoi(temp);

				//Godziny
				read_until(iterator + HEAD_TIME.length() + 3, '.', temp);
				time.tm_mon = std::stoi(temp);

				//Godziny
				read_until(iterator + HEAD_TIME.length() + 6, '.', temp);
				time.tm_year = std::stoi(temp);

				//Godziny
				read_until(iterator + HEAD_TIME.length() + 11, ':', temp);
				time.tm_hour = std::stoi(temp);

				//Minuty
				read_until(iterator + HEAD_TIME.length() + 14, ':', temp);
				time.tm_min = std::stoi(temp);

				//Sekundy
				read_until(iterator + HEAD_TIME.length() + 17, ' ', temp);
				time.tm_sec = std::stoi(temp);
			}

			//Numer Identyfikacyjny (pole obowi¹zkowe)
			if (parse_header(HEAD_SESSION_ID, temp)) { sessionId = std::stoi(temp); }

			//Numer Sekwencyjny (pole obowi¹zkowe)
			if (parse_header(HEAD_SEQ_NUM, temp)) { sequenceNumber = std::stoi(temp); }

			//OPERACJA
			if (parse_header(HEAD_OPERATION, temp)) { operation = temp; }

			//Status 
			else if (parse_header(HEAD_STATUS, temp)) { status = temp; }

			//Liczba
			else if (parse_header(HEAD_NUMBER, temp)) { number = std::stod(temp); }

			//Identyfikator obliczeñ
			else if (parse_header(HEAD_CALC_ID, temp)) { calculationId = std::stoi(temp); }

			//Adres
			else if (parse_header(HEAD_ADDRESS, temp)) { address = temp; }
		}
	}

	//Zerowanie protoko³u
	void clear() {
		time = tm();
		sessionId = NULL;
		sequenceNumber = NULL;
		operation.clear();
		status.clear();
		number = NAN;
		calculationId = NULL;
	}
};
