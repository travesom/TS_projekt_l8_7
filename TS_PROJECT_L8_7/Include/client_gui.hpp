#pragma once
#include "calculator_client.hpp"

class client_gui
{
private:
	calculator_client client_;
	std::string session_id_info_;
	static const std::string action_choice_text;
	static const std::string enter_arguments_text;
	static const std::string out_of_range_text;
	static const std::string calculation_id_text;

	const unsigned menu_box_width_ = 100;
	const unsigned menu_box_height_ = 14;

public:
	client_gui(const std::string& server_address, const unsigned short& port);

	~client_gui();

	static void find_server_text(bool& stop);

	bool start_session();

	bool retry_session_starting() const;

	void main_menu();

	void finish() const;
	
private:
	static uint8_t action_choice_main_menu();

	void calculation_menu();

	static uint8_t action_choice_calculation_menu();

	static std::array<std::string, 2> arg_input_two_add();

	static std::array<std::string, 2> arg_input_two_subt();

	static std::array<std::string, 2> arg_input_two_multp();

	static std::array<std::string, 2> arg_input_two_div();

	static std::array<std::string, 2> arg_input_one_uint_fact();

	void calculation_menu(const std::array<std::string, 2>& args, const text_protocol::operation_enum& operation);

	void history_menu();

	static uint8_t action_choice_history_menu();

	void history_by_session_id();

	void print_history(const std::list<text_protocol>& history) const;

	static void process_operation(const text_protocol& message, std::string& calculation_sign, bool& factorial);

	void process_status(const text_protocol& message, unsigned& argument_number) const;

	void process_arguments(const text_protocol& message, unsigned& argument_number, const std::string& calculation_sign,
	                       bool& factorial) const;

	void history_by_calculation_id();

	unsigned arg_input_one_uint() const;

};
