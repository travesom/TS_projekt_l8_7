#pragma once
#define _WINSOCKAPI_

#include "ThreadSafe.hpp"
#include <Windows.h>
#include <string>
#include <conio.h>

/*
 * Kod bazowany na projekcie z JiPP (semestr 2, 2018):
 * https://github.com/Gunock/Console_RPG_Game
 *
 * Plik: https://github.com/Gunock/Console_RPG_Game/blob/master/src/CManip.cpp
 */

class CONSOLE_MANIP {
private:
	//Funkcje odrzucaj¹ce znaki ------------------------------------------------------------

	//Odrzuca znaki inne ni¿ cyfry
	static const bool check_other_than_num(const char& c) {
		if (c >= '0' && c <= '9') { return false; }
		return true;
	}

	//Odrzuca znaki inne ni¿ litery
	static const bool check_other_that_alf(const char& c) {
		if (c >= 'A' && c <= 'Z') { return false; }
		else if (c >= 'a' && c <= 'z') { return false; }
		return true;
	}
	static const bool check_other_than_alf_y_n(const char& c) {
		if (c == 'N' || c == 'n') { return false; }
		else if (c == 'Y' || c == 'y') { return false; }
		return true;
	}

	//--------------------------------------------------------------------------------------

	//Wprowadzanie danych przez u¿ytkownika z ograniczeniem liczby znaków
	static void input_string(std::string &str, const unsigned int &limit, const bool(*check_function)(const char&)) {
		while (true) {
			cursor_move(-int(str.size()), 0);
			sync_cout << str;

			show_console_cursor(true);
			const char c = _getch();
			show_console_cursor(false);

			check_alt_f4();

			if (c == 0x0d) { if (!str.empty()) { break; } }
			else if (c == 0x08) {	//Jeœli wprowadzono backspace
				if (!str.empty()) {
					cursor_move(-1, 0);
					sync_cout << ' ';
					cursor_move(-1, 0);
					str.pop_back();
				}
			}
			else if (!str.empty()) { if (str.size() == 1 && str[0] == '0') { continue; } }

			if (check_function(c)) { continue; }
			else if (c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z') {
				if (str.size() < limit) {
					cursor_move(1, 0);
					str += c;
				}
			}
		}
	}

	//Zamienia wszystkie litery string'a na du¿e
	static void to_upper(std::string& str) {
		for (char& c : str) {
			if (c >= 97 && c <= 122) { c -= 32; }
		}
	}


public:
	//Funkcje dotycz¹ce kursora ------------------------------------------------------------

	//Uzyskiwanie aktualnej pozycji kursora
	static const COORD cursor_get_pos() {
		CONSOLE_SCREEN_BUFFER_INFO cbsi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi);
		return cbsi.dwCursorPosition;
	}

	//Ustawienie pozycji cursora
	static void cursor_set_pos(const COORD& c) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	}

	//Przemiszczenie kursora
	static void cursor_move(const int& movX, const int& movY) {
		COORD c = cursor_get_pos();
		c.X += movX;
		c.Y += movY;

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	}

	//Zmiana pozycji kursora
	static void cursor_set_pos(const unsigned int& x, const unsigned int& y) {
		COORD c;
		c.X = x;
		c.Y = y;
		cursor_set_pos(c);
	}

	//Ustawianie widocznoœci kursora
	static void show_console_cursor(const bool &showFlag) {
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_CURSOR_INFO     cursorInfo;

		GetConsoleCursorInfo(out, &cursorInfo);
		cursorInfo.bVisible = showFlag; // set the cursor visibility
		SetConsoleCursorInfo(out, &cursorInfo);
	}



	//Funkcje sprawdzania stanów klawiatury ------------------------------------------------

	static const bool check_enter() {
		return GetAsyncKeyState(VK_RETURN) & 0x8000;
	}
	static const bool check_space() {
		return GetAsyncKeyState(VK_SPACE) & 0x8000;
	}
	static const bool check_escape() {
		return GetAsyncKeyState(VK_ESCAPE) & 0x8000;
	}
	static const bool check_shift() {
		return GetAsyncKeyState(VK_SHIFT) & 0x8000;
	}
	static const bool check_ctrl() {
		return GetAsyncKeyState(VK_CONTROL) & 0x8000;
	}
	static const bool check_tilde() {
		return GetAsyncKeyState(VK_OEM_3) & 0x8000;
	}
	//Na direction podajemy kierunek strza³ki
	static const bool check_arrow(std::string direction) {
		to_upper(direction);
		if (direction == "LEFT") { return GetAsyncKeyState(VK_LEFT) & 0x8000; }
		if (direction == "RIGHT") { return GetAsyncKeyState(VK_RIGHT) & 0x8000; }
		if (direction == "UP") { return GetAsyncKeyState(VK_UP) & 0x8000; }
		if (direction == "DOWN") { return GetAsyncKeyState(VK_DOWN) & 0x8000; }
		return false;
	}
	static const bool check_alt() { return GetAsyncKeyState(VK_MENU) & 0x8000; }
	static void check_alt_f4() { if (check_alt() && GetAsyncKeyState(VK_F4) & 0x8000) exit(0); }

	//--------------------------------------------------------------------------------------



	//Czyszczenie bufora wejœciowego
	static void clear_console_input_buffer() {
		const auto ClearingVar1 = new INPUT_RECORD[256];
		DWORD ClearingVar2;
		ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ClearingVar1, 256, &ClearingVar2);
		delete[] ClearingVar1;
	}

	//Czyszczenie konsoli za pomoc¹ funkcji z windows API
	static void clear_console() {
		const COORD topLeft = { 0, 0 };
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO screen;
		DWORD written;

		GetConsoleScreenBufferInfo(console, &screen);
		FillConsoleOutputCharacterA(
			console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
		FillConsoleOutputAttribute(
			console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
			screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
		SetConsoleCursorPosition(console, topLeft);
	}



	//Funkcje wprowadzania danych ----------------------------------------------------------

	//Wprowadzanie cyfr przez u¿ytkownika z ograniczeniem liczby znaków
	static void input_string_digits(std::string &str, const unsigned int &limit) {
		input_string(str, limit, &check_other_than_num);
	}

	//Wprowadzanie liczby typu int (mo¿liwoœæ wprowadzenia '-' jako pierwszy znak)
	static void input_string_int_number(std::string &str, const unsigned int &limit) {
		while (true) {
			cursor_move(-int(str.size()), 0);
			sync_cout << str;

			show_console_cursor(true);
			const char c = _getch();
			show_console_cursor(false);

			check_alt_f4();

			if (c == 0x0d) { if (!str.empty()) { break; } }
			else if (c == 0x08) {	//Jeœli wprowadzono backspace
				if (!str.empty()) {
					cursor_move(-1, 0);
					sync_cout << ' ';
					cursor_move(-1, 0);
					str.pop_back();
				}
			}
			else if (str.length() == 1 && str[0] == '-' && c == '0') { continue; }
			else if (!str.empty()) { if (str.size() == 1 && str[0] == '0') { continue; } }

			if (str.empty() && c == '-') { cursor_move(1, 0); str += c; }
			else if (check_other_than_num(c)) { continue; }
			else if (c >= '0' && c <= '9') {
				if (str.size() < limit) { cursor_move(1, 0); str += c; }
				else if (!str.empty() && str[0] == '-' && str.size() == limit) {
					cursor_move(1, 0);
					str += c;
				}
			}
		}
	}

	//Tak jak powy¿ej z dodatkiem nawiasów dla ujemnych liczb
	static void input_string_int_number_brackets(std::string &str, const unsigned int &limit) {
		while (true) {
			if (!str.empty()) {
				cursor_move(-int(str.size()), 0);
				if (str[0] != '-') {
					sync_cout << str << "  ";
					cursor_move(-2, 0);
				}
				else if (str[0] == '-') {
					if (str.size() > 1) { cursor_move(-1, 0); }
					sync_cout << '(' << str << ")  ";
					cursor_move(-3, 0);
				}
			}
			else { sync_cout << "    "; cursor_move(-4, 0); }

			show_console_cursor(true);
			const char c = _getch();
			show_console_cursor(false);

			check_alt_f4();

			if (c == 0x0d) { //Jeœli wprowadzono enter
				if (str.size() == 1 && str[0] == '-') {
					cursor_move(-1, 0);
					continue;
				}
				else if (!str.empty()) { break; }
			}
			else if (c == 0x08) {	//Jeœli wprowadzono backspace
				if (!str.empty()) {
					cursor_move(-1, 0);
					if (str[0] == '-' && str.size() == 2) { cursor_move(-1, 0); }
					else if (str[0] == '-' && str.size() == 1) { cursor_move(-1, 0); }
					str.pop_back();
				}
				continue;
			}
			else if (str.length() == 1 && str[0] == '-' && c == '0') { cursor_move(-1, 0); continue; }
			else if (!str.empty()) { if (str.size() == 1 && str[0] == '0') { continue; } }

			if (str.empty() && c == '-') { cursor_move(1, 0); str += c; }
			else if (check_other_than_num(c)) { cursor_move(-1, 0); continue; }
			else if (c >= '0' && c <= '9') {
				if (str.size() < limit) {
					cursor_move(1, 0);
					str += c;
				}
				else if (!str.empty() && str[0] == '-' && str.size() == limit) {
					cursor_move(1, 0);
					str += c;
				}
			}
		}
		if (!str.empty()) { if (str[0] == '-') { cursor_move(1, 0); } }
	}

	//--------------------------------------------------------------------------------------



	//Wyœwietlanie elementów tekstowych
	static void print_box(const unsigned int &xPos, const unsigned int &yPos, const unsigned int &width, const unsigned int &height) {
		COORD coordinates1;
		coordinates1.X = xPos;
		coordinates1.Y = yPos;

		COORD coordinates2;
		coordinates2.X = xPos + width + 1;
		coordinates2.Y = yPos;

		for (unsigned int i = 0; i <= height + 1; i++) {
			cursor_set_pos(coordinates1);
			sync_cout << "|";
			coordinates1.Y += 1;

			cursor_set_pos(coordinates2);
			coordinates2.Y += 1;
			sync_cout << "|";
		}

		coordinates1.X = xPos + 1;
		coordinates1.Y = yPos;

		coordinates2.X = xPos + 1;
		coordinates2.Y = yPos + height + 1;

		const std::string equalSigns{ std::string(width, '=') };

		cursor_set_pos(coordinates1);
		sync_cout << equalSigns;

		cursor_set_pos(coordinates2);
		sync_cout << equalSigns;
	}
	static void print_text(const unsigned int &xPos, const unsigned int &yPos, const std::string &text) {
		cursor_set_pos(xPos, yPos);
		sync_cout << text;
	}

	//Wyœwlietlanie napisu pauzy wraz z animacj¹
	static void press_any_key_text(bool& stop) {
		auto timeStart = std::chrono::system_clock::now();
		const std::string searchingText = "Naciœnij dowolny przycisk, aby kontynuowaæ ";
		unsigned int dotNumber = 0;
		std::string dotsText = dotsText;

		const COORD startCursorPosition = cursor_get_pos();

		print_text(startCursorPosition.X, startCursorPosition.Y, searchingText + dotsText);
		while (true) {
			std::chrono::duration<double> time = std::chrono::system_clock::now() - timeStart;
			if (time >= std::chrono::duration<double>(0.5)) {
				dotNumber++;
				if (dotNumber > 3) {
					dotNumber = 0;
					dotsText = "";
				}
				else { dotsText += ". "; }
				print_text(startCursorPosition.X, startCursorPosition.Y, searchingText + "          ");
				print_text(startCursorPosition.X, startCursorPosition.Y, searchingText + dotsText);
				timeStart = std::chrono::system_clock::now();
			}
			if (stop) {
				return;
			}
		}
	}

	//Zast¹pienie system("pause")
	static void press_any_key_pause() {
		show_console_cursor(false);
		bool textStop = false;
		std::thread textThread(&CONSOLE_MANIP::press_any_key_text, std::ref(textStop));

		clear_console_input_buffer();
		_getch(); //Czekanie na naciœniêcie klawisza

		clear_console_input_buffer();
		textStop = true;
		textThread.join();
	}

};
