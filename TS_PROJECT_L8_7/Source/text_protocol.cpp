#include  "text_protocol.hpp"

#include "utils.hpp"

#include <ctime>
#include <iomanip>

using namespace std;

const map<unsigned, std::string> text_protocol::header_text = {
	{head_operation, "OPERATION: "},
	{head_status, "STATUS: "},
	{head_number, "CALCULATION_NUMBER: "},
	{head_seq_num, "SEQUENCE_NUMBER: "},
	{head_session_id, "SESSION_ID: "},
	{head_calc_id, "CALCULATION_ID: "},
	{head_time, "TIME: "},
	{head_address, "ADDRESS: "}
};

const map<unsigned, std::string> text_protocol::status_text = {
	{status_none, "NONE"},
	{status_success, "OPERATION_SUCCESSFUL"},
	{status_out_of_range, "RESULT_OUT_OF_RANGE"},
	{status_forbidden, "ACCESS_DENIED"},
	{status_not_found, "NOT_FOUND"},
	{status_found, "FOUND"},
	{status_history_empty, "HISTORY_EMPTY"}
};

const map<unsigned, std::string> text_protocol::operation_text = {
	{op_none, "NONE"},
	{op_begin, "BEGIN"},
	{op_ack, "ACK"},
	{op_end, "END"},
	{op_add, "ADD"},
	{op_subt, "SUBTRACT"},
	{op_div, "DIVIDE"},
	{op_multp, "MULTIPLY"},
	{op_fact, "FACTORIAL"},
	{op_status, "STATUS"},
	{op_id_session, "SESSION_ID"},
	{op_history_whole, "HISTORY_WHOLE"},
	{op_history_id, "HISTORY_BY_ID"}
};

const map<unsigned, unsigned> text_protocol::field_header_map = {
	{field_no_additional, head_none},
	{field_operation, head_operation},
	{field_number, head_number},
	{field_calculation_id, head_calc_id},
	{field_status, head_status},
	{field_address, head_address}
};

text_protocol::text_protocol(const tm& time, const unsigned& id, const unsigned& sequence_number): text_protocol()
{
	this->time_stamp = time;
	this->session_id = id;
	this->sequence_number = sequence_number;
}

text_protocol::
text_protocol(const tm& time, const unsigned& id, const operation_enum& operation) : text_protocol(time, id, 0)
{
	this->operation = operation;
}

text_protocol::text_protocol(const tm& time, const unsigned& id, const status_enum& status) : text_protocol(time, id, 0)
{
	this->status = status;
}

text_protocol::text_protocol(const string& data)
{
	deserialize(data);
}

text_protocol::~text_protocol() = default;

string text_protocol::serialize(const field_enum& field) const
{
	string result;

	// Adding required fields
	result += header_text.at(head_time) + utils::tm_to_string(time_stamp) + ' '
		+ header_text.at(head_session_id) + to_string(session_id) + ' '
		+ header_text.at(head_seq_num) + to_string(sequence_number) + ' ';

	const header_enum header = header_enum(field_header_map.at(field));
	if (header != head_none)
	{
		result += header_text.at(header);
		if (field == field_operation) { result += operation_text.at(operation); }
		else if (field == field_number) { result += utils::remove_trailing_zeros(to_string(number)); }
		else if (field == field_calculation_id) { result += to_string(calculation_id); }
		else if (field == field_status) { result += status_text.at(status); }
		else if (field == field_address) { result += address; }
	}

	if (MESSAGE_END_SPACE) { result += ' '; }
	return result;
}

text_protocol::field_enum text_protocol::get_field() const
{
	if (operation != op_none) { return field_operation; }
	if (!address.empty()) { return field_address; }
	if (status != status_none) { return field_status; }
	if (calculation_id != unsigned(-1)) { return field_calculation_id; }
	if (!isnan(number)) { return field_number; }
	return field_no_additional;
}

void text_protocol::deserialize(const string& data)
{
	this->clear();
	if (data.empty()) { return; }

	string temp;
	auto find_header = [&data](const string& header, string& temp)
	{
		temp.clear();
		const auto iterator = data.find(header);
		if (iterator != string::npos)
		{
			for (auto i = iterator + header.length(); i < data.size(); i++)
			{
				if (data[i] == '\0' || data[i] == ' ') { break; }
				temp += data[i];
			}
			return true;
		}
		else { return false; }
	};

	// Required fields
	const unsigned pos = unsigned(data.find(header_text.at(head_time)) + header_text.at(head_time).length());
	time_stamp = utils::string_to_tm(data.substr(pos, 20));
	if (find_header(header_text.at(head_session_id), temp)) { session_id = stoi(temp); }
	if (find_header(header_text.at(head_seq_num), temp)) { sequence_number = stoi(temp); }

	// Optional fields
	if (find_header(header_text.at(head_operation), temp))
	{
		operation = operation_enum(utils::reverse_map_lookup(operation_text, temp));
	}
	else if (find_header(header_text.at(head_status), temp))
	{
		status = status_enum(utils::reverse_map_lookup(status_text, temp));
	}
	else if (find_header(header_text.at(head_number), temp)) { number = stod(temp); }
	else if (find_header(header_text.at(head_calc_id), temp)) { calculation_id = stoi(temp); }
	else if (find_header(header_text.at(head_address), temp)) { address = temp; }
}

void text_protocol::clear()
{
	time_stamp = tm();
	session_id = -1;
	sequence_number = -1;
	operation = op_none;
	status = status_none;
	number = NAN;
	calculation_id = -1;
}
