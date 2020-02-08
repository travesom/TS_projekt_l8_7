#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>

/*
 * Source:
 * https:// github.com/Gunock/Console_RPG_Game
 */

class console_manip
{
private:
	static bool check_other_than_num(const char& c);

	static void input_string(std::string& str, const unsigned int& limit, bool (*check_function)(const char&));

	static void to_upper(std::string& str);

public:
	static COORD cursor_get_pos();

	static void cursor_set_pos(const COORD& c);

	static void cursor_move(const int& mov_x, const int& mov_y);

	static void cursor_set_pos(const unsigned int& x, const unsigned int& y);

	static void show_console_cursor(const bool& show_flag);

	static const bool check_enter();

	static const bool check_escape();

	static const bool check_arrow(std::string direction);

	static const bool check_alt();

	static void check_alt_f4();

	static void clear_console_input_buffer();

	static void clear_console();

	static void input_string_digits(std::string& str, const unsigned int& limit);

	static void input_string_int_number(std::string& str, const unsigned int& limit);

	static void input_string_int_number_brackets(std::string& str, const unsigned int& limit);

	static void print_box(const unsigned int& x_pos, const unsigned int& y_pos, const unsigned int& width,
	                      const unsigned int& height);

	static void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const std::string& text);

	static void press_any_key_text(bool& stop);

	static void press_any_key_pause();
};
