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
	unsigned int sessionId;             //Identyfikator sesji
	unsigned int sequenceNumber; //Numer sekwencyjny (pole obowiązkowe)

	std::string operation;      //Pole operacji
	std::string status;         //Pole statusu
	std::string address;        //Pole adresu (wykorzystywane przy szukaniu serwera)
	double number;              //Pole liczby
	unsigned int calculationId; //Identyfikator obliczeń

	//Konstruktor domyślny
	TextProtocol() : time(), sessionId(NULL), sequenceNumber(NULL), operation(""), status(""), number(NAN), calculationId(NULL) {};

	TextProtocol(const tm& time_, const unsigned int& id_, const unsigned int& sequenceNumber_) : TextProtocol() {
		time = time_;
		sessionId = id_;
		sequenceNumber = sequenceNumber_;
	}

	explicit TextProtocol(const std::string& data) { from_string(data); }

	virtual ~TextProtocol() = default;;

	//Serializacja
	std::string to_string(const int& field) const {
		std::string result;

		//Dodanie znacznika czasowego (pole obowiązkowe)
		{
			std::stringstream resultStream;
			result += HEAD_TIME;
			resultStream << time;
			std::string temp;
			resultStream >> temp;
			result += temp + ' ';
		}

		//Dodanie identyfikatora sesji (pole obowiązkowe)
		result += HEAD_SESSION_ID + std::to_string(sessionId) + ' ';

		//Pole numer sekwencyjny (pole obowiązkowe)
		result += HEAD_SEQNUM; result += std::to_string(sequenceNumber) + ' ';

		//Pole numer operacja
		if (field == FIELD_OPERATION) { result += HEAD_OPERATION + operation; }
		//Pole liczba
		else if (field == FIELD_NUMBER) {
			std::string numberStr = std::to_string(number);
			double_remove_end_zero(numberStr);
			result += HEAD_NUMBER + numberStr;
		}
		//Pole sessionId obliczeń
		else if (field == FIELD_CALCULATION_ID) { result += HEAD_CALC_ID + std::to_string(calculationId); }
		//Pole status
		else if (field == FIELD_STATUS) { result += HEAD_STATUS + status; }
		//Pole status
		else if (field == FIELD_ADDRESS) { result += HEAD_ADDRESS + address; }

		result += ' ';

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
			auto iterator = data.find(HEAD_TIME);
			std::string temp;

			//Czas (pole obowiązkowe)
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

			//Numer Identyfikacyjny (pole obowiązkowe)
			{
				temp.clear();
				iterator = data.find(HEAD_SESSION_ID);
				for (auto i = iterator + HEAD_SESSION_ID.length(); i < data.size(); i++) {
					if (data[i] == '\0' || data[i] == ' ') { break; }
					temp += data[i];
				}
				temp += ' ';
				sessionId = std::stoi(temp);
			}

			//Numer Sekwencyjny (pole obowiązkowe)
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

			//OPERACJA
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

			//Identyfikator obliczeń
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

			//Adres
			{
				iterator = data.find(HEAD_ADDRESS);
				if (iterator != std::string::npos) {
					temp.clear();
					for (auto i = iterator + HEAD_ADDRESS.length(); i < data.size(); i++) {
						if (data[i] == '\0' || data[i] == ' ') { break; }
						temp += data[i];
					}
					address = temp;
				}
			}
		}
	}

	//Zerowanie protokołu
	void clear() {
		operation.clear();
		status.clear();
		number = NAN;
		sequenceNumber = NULL;
		sessionId = NULL;
		calculationId = NULL;
		time = tm();
	}
};
