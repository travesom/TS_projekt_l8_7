#pragma once
#include <iostream>
#include <map>

static const bool MESSAGE_END_SPACE = true;

class text_protocol final
{
public:

	enum header_enum : unsigned
	{
		head_none,
		head_operation,
		head_status,
		head_number,
		head_seq_num,
		head_session_id,
		head_calc_id,
		head_time,
		head_address
	};

	enum status_enum : unsigned
	{
		status_none,
		status_success,
		status_out_of_range,
		status_forbidden,
		status_not_found,
		status_found,
		status_history_empty
	};

	enum operation_enum : unsigned
	{
		op_none,
		op_begin,
		op_ack,
		op_end,
		op_add,
		op_subt,
		op_div,
		op_multp,
		op_fact,
		op_status,
		op_id_session,
		op_history_whole,
		op_history_id
	};

	enum field_enum : unsigned
	{
		field_no_additional,
		field_operation,
		field_number,
		field_calculation_id,
		field_status,
		field_address
	};


	static const std::map<unsigned, std::string> header_text;
	static const std::map<unsigned, std::string> status_text;
	static const std::map<unsigned, std::string> operation_text;
	static const std::map<unsigned, unsigned> field_header_map;

	tm time_stamp = tm();
	unsigned session_id = -1;
	unsigned sequence_number = -1;

	operation_enum operation = op_none;
	status_enum status = status_none;
	std::string address = "";
	double number = NAN;
	unsigned calculation_id = -1;

	text_protocol() = default;

	text_protocol(const tm& time, const unsigned int& id, const unsigned int& sequence_number);

	text_protocol(const tm& time, const unsigned int& id, const operation_enum& operation);

	text_protocol(const tm& time, const unsigned int& id, const status_enum& status);


	explicit text_protocol(const std::string& data);


	~text_protocol();


	std::string serialize(const field_enum& field) const;

	field_enum get_field() const;


	void deserialize(const std::string& data);


	void clear();
};
