#include "console_manip.hpp"

#include "sync_cout.hpp"

#include <conio.h>

using namespace std;

bool console_manip::check_other_than_num(const char& c)
{
	if (c >= '0' && c <= '9') { return false; }
	return true;
}

void console_manip::input_string(string& str, const unsigned& limit, bool (* check_function)(const char&))
{
	while (true)
	{
		cursor_move(-int(str.size()), 0);
		s_cout << str;

		show_console_cursor(true);
		const char c = char(_getch());
		show_console_cursor(false);

		check_alt_f4();

		if (c == 0x0d) { if (!str.empty()) { break; } }
			// backspace
		else if (c == 0x08)
		{
			if (!str.empty())
			{
				cursor_move(-1, 0);
				s_cout << ' ';
				cursor_move(-1, 0);
				str.pop_back();
			}
		}
		else if (!str.empty()) { if (str.size() == 1 && str[0] == '0') { continue; } }

		if (check_function(c)) { continue; }
		else if (c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
		{
			if (str.size() < limit)
			{
				cursor_move(1, 0);
				str += c;
			}
		}
	}
}

void console_manip::to_upper(string& str)
{
	for (char& c : str)
	{
		if (c >= 97 && c <= 122) { c -= 32; }
	}
}

COORD console_manip::cursor_get_pos()
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi);
	return cbsi.dwCursorPosition;
}

void console_manip::cursor_set_pos(const COORD& c)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void console_manip::cursor_move(const int& mov_x, const int& mov_y)
{
	COORD c = cursor_get_pos();
	c.X += mov_x;
	c.Y += mov_y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void console_manip::cursor_set_pos(const unsigned& x, const unsigned& y)
{
	COORD c;
	c.X = x;
	c.Y = y;
	cursor_set_pos(c);
}

void console_manip::show_console_cursor(const bool& show_flag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = show_flag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

const bool console_manip::check_enter()
{
	return GetAsyncKeyState(VK_RETURN) & 0x8000;
}

const bool console_manip::check_escape()
{
	return GetAsyncKeyState(VK_ESCAPE) & 0x8000;
}

const bool console_manip::check_arrow(string direction)
{
	to_upper(direction);
	if (direction == "LEFT") { return GetAsyncKeyState(VK_LEFT) & 0x8000; }
	if (direction == "RIGHT") { return GetAsyncKeyState(VK_RIGHT) & 0x8000; }
	if (direction == "UP") { return GetAsyncKeyState(VK_UP) & 0x8000; }
	if (direction == "DOWN") { return GetAsyncKeyState(VK_DOWN) & 0x8000; }
	return false;
}

const bool console_manip::check_alt()
{
	return GetAsyncKeyState(VK_MENU) & 0x8000;
}

void console_manip::check_alt_f4()
{
	if (check_alt() && GetAsyncKeyState(VK_F4) & 0x8000) exit(0);
}

void console_manip::clear_console_input_buffer()
{
	const auto ClearingVar1 = new INPUT_RECORD[256];
	DWORD ClearingVar2;
	ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ClearingVar1, 256, &ClearingVar2);
	delete[] ClearingVar1;
}

void console_manip::clear_console()
{
	const COORD top_left = {0, 0};
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, top_left, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, top_left, &written
	);
	SetConsoleCursorPosition(console, top_left);
}

void console_manip::input_string_digits(string& str, const unsigned& limit)
{
	input_string(str, limit, &check_other_than_num);
}

void console_manip::input_string_int_number(string& str, const unsigned& limit)
{
	while (true)
	{
		cursor_move(-int(str.size()), 0);
		s_cout << str;

		show_console_cursor(true);
		const char c = _getch();
		show_console_cursor(false);

		check_alt_f4();

		if (c == 0x0d) { if (!str.empty()) { break; } }
			// backspace
		else if (c == 0x08)
		{
			if (!str.empty())
			{
				cursor_move(-1, 0);
				s_cout << ' ';
				cursor_move(-1, 0);
				str.pop_back();
			}
		}
		else if (str.length() == 1 && str[0] == '-' && c == '0') { continue; }
		else if (!str.empty()) { if (str.size() == 1 && str[0] == '0') { continue; } }

		if (str.empty() && c == '-')
		{
			cursor_move(1, 0);
			str += c;
		}
		else if (check_other_than_num(c)) { continue; }
		else if (c >= '0' && c <= '9')
		{
			if (str.size() < limit)
			{
				cursor_move(1, 0);
				str += c;
			}
			else if (!str.empty() && str[0] == '-' && str.size() == limit)
			{
				cursor_move(1, 0);
				str += c;
			}
		}
	}
}

void console_manip::input_string_int_number_brackets(string& str, const unsigned& limit)
{
	while (true)
	{
		if (!str.empty())
		{
			cursor_move(-int(str.size()), 0);
			if (str[0] != '-')
			{
				s_cout << str << "  ";
				cursor_move(-2, 0);
			}
			else if (str[0] == '-')
			{
				if (str.size() > 1) { cursor_move(-1, 0); }
				s_cout << '(' << str << ")  ";
				cursor_move(-3, 0);
			}
		}
		else
		{
			s_cout << "    ";
			cursor_move(-4, 0);
		}

		show_console_cursor(true);
		const char c = _getch();
		show_console_cursor(false);

		check_alt_f4();

		// enter
		if (c == 0x0d)
		{
			if (str.empty()) { continue; }
			if (str.size() == 1 && str[0] == '-')
			{
				cursor_move(-1, 0);
				continue;
			}
			break;
		}
		// backspace
		else if (c == 0x08)
		{
			if (!str.empty())
			{
				cursor_move(-1, 0);
				if (str[0] == '-' && str.size() == 2) { cursor_move(-1, 0); }
				else if (str[0] == '-' && str.size() == 1) { cursor_move(-1, 0); }
				str.pop_back();
			}
			continue;
		}
		else if (str.length() == 1 && str[0] == '-' && c == '0')
		{
			cursor_move(-1, 0);
			continue;
		}
		else if (!str.empty()) { if (str.size() == 1 && str[0] == '0') { continue; } }

		if (str.empty() && c == '-')
		{
			cursor_move(1, 0);
			str += c;
		}
		else if (check_other_than_num(c))
		{
			cursor_move(-1, 0);
			continue;
		}
		else if (c >= '0' && c <= '9')
		{
			if (str.size() < limit)
			{
				cursor_move(1, 0);
				str += c;
			}
			else if (!str.empty() && str[0] == '-' && str.size() == limit)
			{
				cursor_move(1, 0);
				str += c;
			}
		}
	}
	if (!str.empty()) { if (str[0] == '-') { cursor_move(1, 0); } }
}

void console_manip::print_box(const unsigned& x_pos, const unsigned& y_pos, const unsigned& width,
                              const unsigned& height)
{
	COORD coordinates1;
	coordinates1.X = x_pos;
	coordinates1.Y = y_pos;

	COORD coordinates2;
	coordinates2.X = x_pos + width + 1;
	coordinates2.Y = y_pos;

	for (unsigned i = 0; i <= height + 1; i++)
	{
		cursor_set_pos(coordinates1);
		s_cout << "|";
		coordinates1.Y += 1;

		cursor_set_pos(coordinates2);
		coordinates2.Y += 1;
		s_cout << "|";
	}

	coordinates1.X = x_pos + 1;
	coordinates1.Y = y_pos;

	coordinates2.X = x_pos + 1;
	coordinates2.Y = y_pos + height + 1;

	const string equalSigns{string(width, '=')};

	cursor_set_pos(coordinates1);
	s_cout << equalSigns;

	cursor_set_pos(coordinates2);
	s_cout << equalSigns;
}

void console_manip::print_text(const unsigned& x_pos, const unsigned& y_pos, const string& text)
{
	cursor_set_pos(x_pos, y_pos);
	s_cout << text;
}

void console_manip::press_any_key_text(bool& stop)
{
	auto time_start = chrono::system_clock::now();
	const string searching_text = "Press any key to continue ";
	unsigned dot_number = 0;
	string dots_text;

	const COORD start_cursor_position = cursor_get_pos();

	print_text(start_cursor_position.X, start_cursor_position.Y, searching_text + dots_text);
	while (true)
	{
		chrono::duration<double> time = chrono::system_clock::now() - time_start;
		if (time >= chrono::duration<double>(0.5))
		{
			dot_number++;
			if (dot_number > 3)
			{
				dot_number = 0;
				dots_text = "";
			}
			else { dots_text += ". "; }
			print_text(start_cursor_position.X, start_cursor_position.Y, searching_text + "          ");
			print_text(start_cursor_position.X, start_cursor_position.Y, searching_text + dots_text);
			time_start = chrono::system_clock::now();
		}
		if (stop)
		{
			return;
		}
	}
}

void console_manip::press_any_key_pause()
{
	show_console_cursor(false);
	bool text_stop = false;
	thread text_thread(&console_manip::press_any_key_text, ref(text_stop));

	clear_console_input_buffer();
	_getch();

	clear_console_input_buffer();
	text_stop = true;
	text_thread.join();
}
