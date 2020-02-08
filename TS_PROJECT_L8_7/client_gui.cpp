#include "client_gui.hpp"

#include "console_manip.hpp"
#include "sync_cout.hpp"
#include "utils.hpp"

#include <thread>


using namespace std;

const string client_gui::action_choice_text = "Choose action: ";
const string client_gui::enter_arguments_text = "Enter arguments: ";
const string client_gui::out_of_range_text = "Out of range!";
const string client_gui::calculation_id_text = "Calculation ID: ";

client_gui::client_gui(const string& server_address, const unsigned short& port) : client_(server_address, port)
{
	console_manip::show_console_cursor(false);
}

client_gui::~client_gui()
{
	console_manip::show_console_cursor(true);
}

void client_gui::find_server_text(bool& stop)
{
	auto time_start = chrono::system_clock::now();
	const string searching_text = "Finding server";
	unsigned dot_number = 0;
	string dots_text;

	console_manip::print_text(3, 2, searching_text + dots_text);
	while (true)
	{
		chrono::duration<double> time = chrono::system_clock::now() - time_start;
		if (time >= chrono::duration<double>(0.5))
		{
			dot_number++;
			if (dot_number > 3)
			{
				dot_number = 0;
				dots_text.clear();
			}
			else { dots_text += ". "; }
			console_manip::print_text(3, 2, searching_text + "                  ");
			console_manip::print_text(3, 2, searching_text + dots_text);
			time_start = chrono::system_clock::now();
		}
		if (stop)
		{
			return;
		}
	}
}

bool client_gui::start_session()
{
	console_manip::cursor_set_pos(1, 1);
	console_manip::clear_console();

	bool text_stop = false;
	thread text_thread(&client_gui::find_server_text, ref(text_stop));

	uint8_t fail_count = 0;
	bool find_success = false;
	while (fail_count <= 10)
	{
		if (!client_.find_server())
		{
			console_manip::print_text(3 + 2 * fail_count, 1, "X");
			fail_count++;
		}
		else
		{
			find_success = true;
			break;
		}
		Sleep(100);
	}

	session_id_info_ = "Session ID: " + to_string(client_.session_id);

	text_stop = true;
	text_thread.join();
	return find_success;
}

bool client_gui::retry_session_starting() const
{
	const string question = "Do you want to start another session?";
	string no = " No";
	string yes = " Yes";
	const unsigned box_width = 80;
	const unsigned box_height = 20;

	console_manip::cursor_set_pos(1, console_manip::cursor_get_pos().Y);

	bool choice = false;

	console_manip::clear_console();
	console_manip::print_box(0, 0, box_width, box_height);
	console_manip::show_console_cursor(false);

	console_manip::print_box(0, 0, box_width, box_height);

	const string session_id_info = "Session ID: " + to_string(client_.session_id);
	console_manip::print_text(box_width - session_id_info.length() - 2, 1, session_id_info);

	console_manip::print_text(box_width / 2 - question.length() / 2, box_height / 2 - 2, question);

	while (true)
	{
		if (choice)
		{
			no[0] = ' ';
			yes[0] = '>';
		}
		else
		{
			no[0] = '>';
			yes[0] = ' ';
		}
		string yes_no = no + "   " + yes;
		console_manip::print_text(box_width / 2 - yes_no.length() / 2, box_height / 2, yes_no);
		console_manip::clear_console_input_buffer();

		if (console_manip::check_arrow("LEFT") && choice) { choice = false; }
		else if (console_manip::check_arrow("RIGHT") && !choice) { choice = true; }
		else if (console_manip::check_enter()) { break; }
	}
	return choice;
}

void client_gui::main_menu()
{
	session_id_info_ = "Session ID: " + to_string(client_.session_id);

	while (true)
	{
		console_manip::clear_console();
		console_manip::show_console_cursor(false);
		console_manip::print_box(0, 0, menu_box_width_, menu_box_height_);
		console_manip::print_text(menu_box_width_ - session_id_info_.length() - 2, 1, session_id_info_);
		console_manip::print_text(2, 2, action_choice_text);

		const uint8_t choice = action_choice_main_menu();

		if (choice == 1)
		{
			client_.end_session();
			break;
		}
		else if (choice == 2) { calculation_menu(); }
		else if (choice == 3) { history_menu(); }
	}
}

uint8_t client_gui::action_choice_main_menu()
{
	string disconnect = " End session";
	string calculate = " Calculation";
	string history = " History";

	uint8_t choice = 1;
	while (true)
	{
		disconnect[0] = ' ';
		calculate[0] = ' ';
		history[0] = ' ';
		if (choice == 1) { disconnect[0] = '>'; }
		else if (choice == 2) { calculate[0] = '>'; }
		else if (choice == 3) { history[0] = '>'; }

		console_manip::print_text(2, 4, disconnect);
		console_manip::print_text(2, 5, calculate);
		console_manip::print_text(2, 6, history);

		console_manip::clear_console_input_buffer();

		if (console_manip::check_arrow("UP") && choice > 1) { choice--; }
		else if (console_manip::check_arrow("DOWN") && choice < 3) { choice++; }
		else if (console_manip::check_enter()) { break; }
	}
	return choice;
}

void client_gui::calculation_menu()
{
	while (true)
	{
		console_manip::clear_console();
		console_manip::show_console_cursor(false);
		console_manip::print_box(0, 0, menu_box_width_, menu_box_height_);
		console_manip::print_text(menu_box_width_ - session_id_info_.length() - 2, 1, session_id_info_);
		console_manip::print_text(2, 2, action_choice_text);

		const uint8_t choice = action_choice_calculation_menu();

		array<string, 2> args;
		text_protocol::operation_enum operation;

		if (choice == 1) { break; }
		else if (choice == 2)
		{
			args = arg_input_two_add();
			operation = text_protocol::op_add;
		}
		else if (choice == 3)
		{
			args = arg_input_two_subt();
			operation = text_protocol::op_subt;
		}
		else if (choice == 4)
		{
			args = arg_input_two_multp();
			operation = text_protocol::op_multp;
		}
		else if (choice == 5)
		{
			args = arg_input_two_div();
			operation = text_protocol::op_div;
		}
		else if (choice == 6)
		{
			args = arg_input_one_uint_fact();
			operation = text_protocol::op_fact;
		}
		calculation_menu(args, operation);
	}
}

uint8_t client_gui::action_choice_calculation_menu()
{
	string go_back_text = " Back";
	string addition_text = " Add two numbers";
	string subtraction_text = " Subtract two number";
	string multiplicationText = " Multiply two numbers";
	string division_text = " Divide two numbers";
	string factorial_text = " Factorial";

	uint8_t choice = 1;
	while (true)
	{
		go_back_text[0] = ' ';
		addition_text[0] = ' ';
		subtraction_text[0] = ' ';
		multiplicationText[0] = ' ';
		division_text[0] = ' ';
		factorial_text[0] = ' ';
		if (choice == 1) { go_back_text[0] = '>'; }
		else if (choice == 2) { addition_text[0] = '>'; }
		else if (choice == 3) { subtraction_text[0] = '>'; }
		else if (choice == 4) { multiplicationText[0] = '>'; }
		else if (choice == 5) { division_text[0] = '>'; }
		else if (choice == 6) { factorial_text[0] = '>'; }

		console_manip::print_text(2, 4, go_back_text);
		console_manip::print_text(2, 5, addition_text);
		console_manip::print_text(2, 6, subtraction_text);
		console_manip::print_text(2, 7, multiplicationText);
		console_manip::print_text(2, 8, division_text);
		console_manip::print_text(2, 9, factorial_text);

		console_manip::clear_console_input_buffer();

		if (console_manip::check_escape())
		{
			choice = 1;
			break;
		}
		else if (console_manip::check_arrow("UP") && choice > 1) { choice--; }
		else if (console_manip::check_arrow("DOWN") && choice < 6) { choice++; }
		else if (console_manip::check_enter()) { break; }
	}
	return choice;
}

array<string, 2> client_gui::arg_input_two_add()
{
	array<string, 2> args;
	unsigned arg_num = 0;
	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 2);
	while (true)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y);
		s_cout << enter_arguments_text << args[0] << (arg_num == 1 ? " + " + args[1] : "");
		if (arg_num == 0) { console_manip::input_string_int_number(args[arg_num], 10); }
		else if (arg_num == 1) { console_manip::input_string_int_number_brackets(args[arg_num], 10); }
		if (stod(args[arg_num]) < 2147483647 && stod(args[arg_num]) > -2147483647)
		{
			const COORD cursor_pos = console_manip::cursor_get_pos();
			console_manip::print_text(2, console_manip::cursor_get_pos().Y + 1, "                     ");
			console_manip::cursor_set_pos(cursor_pos);
			if (arg_num == 1) { break; }
			arg_num++;
		}
		else
		{
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
			s_cout << out_of_range_text;
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y - 1);
		}
	}
	return args;
}

array<string, 2> client_gui::arg_input_two_subt()
{
	array<string, 2> args;
	unsigned arg_num = 0;
	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 2);
	while (true)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y);
		s_cout << enter_arguments_text << args[0] << (arg_num == 1 ? " - " + args[1] : "");
		if (arg_num == 0) { console_manip::input_string_int_number(args[arg_num], 10); }
		else if (arg_num == 1) { console_manip::input_string_int_number_brackets(args[arg_num], 10); }
		if (stod(args[arg_num]) < 2147483647 && stod(args[arg_num]) > -2147483647)
		{
			const COORD cursor_pos = console_manip::cursor_get_pos();
			console_manip::print_text(2, console_manip::cursor_get_pos().Y + 1, "                     ");
			console_manip::cursor_set_pos(cursor_pos);
			if (arg_num == 1) { break; }
			arg_num++;
		}
		else
		{
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
			s_cout << out_of_range_text;
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y - 1);
		}
	}
	return args;
}

array<string, 2> client_gui::arg_input_two_multp()
{
	array<string, 2> args;
	unsigned arg_num = 0;
	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 2);
	while (true)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y);
		s_cout << enter_arguments_text << args[0] << (arg_num == 1 ? " * " + args[1] : "");
		if (arg_num == 0) { console_manip::input_string_int_number(args[arg_num], 10); }
		else if (arg_num == 1) { console_manip::input_string_int_number_brackets(args[arg_num], 10); }
		if (stod(args[arg_num]) < 2147483647 && stod(args[arg_num]) > -2147483647)
		{
			const COORD cursor_pos = console_manip::cursor_get_pos();
			console_manip::print_text(2, console_manip::cursor_get_pos().Y + 1, "                     ");
			console_manip::cursor_set_pos(cursor_pos);
			if (arg_num == 1) { break; }
			arg_num++;
		}
		else
		{
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
			s_cout << out_of_range_text;
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y - 1);
		}
	}
	return args;
}

array<string, 2> client_gui::arg_input_two_div()
{
	array<string, 2> args;
	unsigned arg_num = 0;
	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 2);
	while (true)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y);
		s_cout << enter_arguments_text << args[0] << (arg_num == 1 ? " / " + args[1] : "");
		if (arg_num == 0) { console_manip::input_string_int_number(args[arg_num], 10); }
		else if (arg_num == 1) { console_manip::input_string_int_number_brackets(args[arg_num], 10); }

		if (arg_num == 1 && stod(args[arg_num]) == 0)
		{
			const COORD cursor_pos = console_manip::cursor_get_pos();
			console_manip::print_text(2, console_manip::cursor_get_pos().Y + 1, "Cannot divide by zero!");
			console_manip::cursor_set_pos(cursor_pos);
		}
		else if (stod(args[arg_num]) < 2147483647 && stod(args[arg_num]) > -2147483647)
		{
			const COORD cursor_pos = console_manip::cursor_get_pos();
			console_manip::print_text(2, console_manip::cursor_get_pos().Y + 1, "                              ");
			console_manip::cursor_set_pos(cursor_pos);
			if (arg_num == 1) { break; }
			arg_num++;
		}
		else
		{
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
			s_cout << out_of_range_text;
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y - 1);
		}
	}
	return args;
}

array<string, 2> client_gui::arg_input_one_uint_fact()
{
	array<string, 2> args;
	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 2);
	while (true)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y);
		s_cout << enter_arguments_text << args[0];
		console_manip::input_string_digits(args[0], 10);

		if (stod(args[0]) < 4294967295)
		{
			const unsigned current_x_pos = console_manip::cursor_get_pos().X;
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
			s_cout << "                     ";
			console_manip::cursor_set_pos(current_x_pos, console_manip::cursor_get_pos().Y - 1);
			s_cout << '!';
			break;
		}
		else
		{
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
			s_cout << out_of_range_text;
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y - 1);
		}
	}
	return args;
}

void client_gui::calculation_menu(const array<string, 2>& args, const text_protocol::operation_enum& operation)
{
	list<text_protocol> received_messages = client_.calculate(args[0], args[1], operation);
	for (const text_protocol& protocol : received_messages)
	{
		if (protocol.operation == text_protocol::op_status)
		{
			s_cout << " = ";
		}
		else if (protocol.status == text_protocol::status_success) { continue; }
		else if (protocol.get_field() == text_protocol::field_number)
		{
			const string number_str = utils::remove_trailing_zeros(to_string(protocol.number));
			s_cout << (protocol.number >= 0 ? number_str : "(" + number_str + ')');
		}
		else if (protocol.status == text_protocol::status_out_of_range) { s_cout << out_of_range_text; }
		else if (protocol.get_field() == text_protocol::field_calculation_id)
		{
			s_cout << " | " << calculation_id_text << protocol.calculation_id << '\n';
		}
	}
	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
	console_manip::press_any_key_pause();
}

void client_gui::history_menu()
{
	while (true)
	{
		console_manip::clear_console();
		console_manip::show_console_cursor(false);
		console_manip::print_box(0, 0, menu_box_width_, menu_box_height_);
		console_manip::print_text(menu_box_width_ - session_id_info_.length() - 2, 1, session_id_info_);
		console_manip::print_text(2, 2, action_choice_text);

		const uint8_t choice = action_choice_history_menu();

		if (choice == 1) { break; }
		else if (choice == 2) { history_by_session_id(); }
		else if (choice == 3) { history_by_calculation_id(); }
	}
}

uint8_t client_gui::action_choice_history_menu()
{
	byte choice = 1;
	string go_back_text = " Back";
	string whole_history = " Display whole history";
	string by_calc_id = " Display calculation with given ID";

	while (true)
	{
		go_back_text[0] = ' ';
		whole_history[0] = ' ';
		by_calc_id[0] = ' ';
		if (choice == 1) { go_back_text[0] = '>'; }
		else if (choice == 2) { whole_history[0] = '>'; }
		else if (choice == 3) { by_calc_id[0] = '>'; }

		console_manip::print_text(2, 4, go_back_text);
		console_manip::print_text(2, 5, whole_history);
		console_manip::print_text(2, 6, by_calc_id);

		console_manip::clear_console_input_buffer();

		if (console_manip::check_escape())
		{
			choice = 1;
			break;
		}
		else if (console_manip::check_arrow("UP") && choice > 1) { choice--; }
		else if (console_manip::check_arrow("DOWN") && choice < 3) { choice++; }
		else if (console_manip::check_enter()) { break; }
	}
	return choice;
}

void client_gui::history_by_session_id()
{
	const list<text_protocol> history = client_.get_history_by_session_id();

	print_history(history);

	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
	console_manip::press_any_key_pause();
}

void client_gui::print_history(const list<text_protocol>& history) const
{
	string calculation_sign;
	unsigned argument_number = 1;
	bool factorial = false;

	if (history.empty()) { return; }

	console_manip::clear_console();
	console_manip::show_console_cursor(false);

	unsigned equation_number = 0;
	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 3);

	for (const text_protocol& message : history)
	{
		if (message.get_field() == text_protocol::field_operation)
		{
			process_operation(message, calculation_sign, factorial);
		}
		else if (message.get_field() == text_protocol::field_status) { process_status(message, argument_number); }
		else if (message.get_field() == text_protocol::field_calculation_id)
		{
			console_manip::cursor_set_pos(menu_box_width_ - 40, console_manip::cursor_get_pos().Y);
			console_manip::print_text(2, console_manip::cursor_get_pos().Y,
			                          calculation_id_text + to_string(message.calculation_id) + "\n");
			equation_number++;
		}
		else if (message.get_field() == text_protocol::field_number)
		{
			process_arguments(message, argument_number, calculation_sign, factorial);
		}
	}

	const COORD cursor_pos = console_manip::cursor_get_pos();
	console_manip::print_box(0, 0, menu_box_width_, equation_number + 4);
	console_manip::print_text(menu_box_width_ - session_id_info_.length() - 2, 1, session_id_info_);
	console_manip::cursor_set_pos(cursor_pos);
}

void client_gui::process_operation(const text_protocol& message, string& calculation_sign, bool& factorial)
{
	if (message.operation == text_protocol::op_fact)
	{
		calculation_sign = "!";
		factorial = true;
	}
	else if (message.operation == text_protocol::op_add) { calculation_sign = " + "; }
	else if (message.operation == text_protocol::op_subt) { calculation_sign = " - "; }
	else if (message.operation == text_protocol::op_multp) { calculation_sign = " * "; }
	else if (message.operation == text_protocol::op_div) { calculation_sign = " / "; }
}

void client_gui::process_status(const text_protocol& message, unsigned& argument_number) const
{
	if (message.status == text_protocol::status_found) { return; }
	if (message.status == text_protocol::status_out_of_range)
	{
		s_cout << " = out of range";
		argument_number = 1;
	}
	else if (message.status == text_protocol::status_forbidden)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
		s_cout << "Access denied!";
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
	}
	else if (message.status == text_protocol::status_history_empty)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
		s_cout << "History empty!";
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
	}
	else if (message.status == text_protocol::status_not_found)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
		s_cout << "Not found!";
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
	}
	else if (message.status == text_protocol::status_success) { s_cout << " = "; }
}

void client_gui::process_arguments(const text_protocol& message, unsigned& argument_number,
                                   const string& calculation_sign,
                                   bool& factorial) const
{
	if (argument_number == 1)
	{
		const string number_str = utils::remove_trailing_zeros(to_string(message.number)) + calculation_sign;
		console_manip::print_text(40, console_manip::cursor_get_pos().Y, "| " + number_str);
		argument_number++;
		if (factorial)
		{
			argument_number++;
			factorial = false;
		}
	}
	else if (argument_number == 2)
	{
		const string number_str = utils::remove_trailing_zeros(to_string(message.number));
		const double number_double = stod(number_str);
		s_cout << (number_double >= 0 ? number_str : "(" + number_str + ')');
		argument_number++;
	}
	else if (argument_number == 3)
	{
		const string number_str = utils::remove_trailing_zeros(to_string(message.number));
		const double number_double = stod(number_str);
		s_cout << (number_double >= 0 ? number_str : "(" + number_str + ')');
		argument_number = 1;
	}
}

void client_gui::history_by_calculation_id()
{
	const unsigned calculation_id = arg_input_one_uint();

	const list<text_protocol> history = client_.get_history_by_calculation_id(calculation_id);

	print_history(history);

	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
	console_manip::press_any_key_pause();
}

unsigned client_gui::arg_input_one_uint() const
{
	string arg;
	console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 2);
	while (true)
	{
		console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y);
		s_cout << "Enter calculation ID: " << arg;
		console_manip::input_string_digits(arg, 10);

		if (stod(arg) < 4294967295)
		{
			const unsigned current_x_pos = console_manip::cursor_get_pos().X;
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
			s_cout << "                     ";
			console_manip::cursor_set_pos(current_x_pos, console_manip::cursor_get_pos().Y - 1);
			break;
		}
		else
		{
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y + 1);
			s_cout << "Out of range!";
			console_manip::cursor_set_pos(2, console_manip::cursor_get_pos().Y - 1);
		}
	}
	return stoi(arg);
}

void client_gui::finish() const
{
	const unsigned box_width = 80;
	const unsigned box_height = 20;
	WSACleanup();
	console_manip::cursor_set_pos(box_width / 4, box_height / 2);
	console_manip::press_any_key_pause();
}
