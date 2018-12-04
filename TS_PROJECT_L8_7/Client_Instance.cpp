#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996)

#include "Client.h"

int main() {
	std::string question;
	std::string no = " Nie";
	std::string yes = " Tak";

	system("chcp 1250");

	ClientUDP client(PORT_TO_SET);

	//Pętla nawiązywania sesji
	const unsigned int boxWidth = 80;
	const unsigned int boxHeight = 20;

	CONSOLE_MANIP::clear_console();
	while (true) {
		CONSOLE_MANIP::cursor_set_pos(1, 1);
		if (!client.start_session()) { question = "Czy chcesz ponownie spróbować rozpocząć sesję?"; }
		else { question = "Czy chcesz wznowić sesję?"; }

		CONSOLE_MANIP::cursor_set_pos(1, CONSOLE_MANIP::cursor_get_pos().Y);

		bool choice = false;

		//Wyłączenie kursora, wyczyszczenie konsoli
		CONSOLE_MANIP::clear_console();
		CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);
		CONSOLE_MANIP::show_console_cursor(false);

		//Wyświetlenie obramowania na konsoli
		CONSOLE_MANIP::print_box(0, 0, boxWidth, boxHeight);

		//Wyświetlenie identyfikatora sesji
		std::string sessionIdInfo = "Identyfikator sesji: " + std::to_string(client.sessionId);
		CONSOLE_MANIP::print_text(boxWidth - sessionIdInfo.length() - 2, 1, sessionIdInfo);

		//Wyświetlenie zapytania o kolejną sesję
		CONSOLE_MANIP::print_text(boxWidth / 2 - question.length() / 2, boxHeight / 2 - 2, question);

		//Pętla wyboru (strzałkami)
		while (true) {
			//DODAWANIE wskaźnika wybranej opcji
			if (choice) {
				no[0] = ' ';
				yes[0] = '>';
			}
			else {
				no[0] = '>';
				yes[0] = ' ';
			}
			//Wyświetlanie opcji
			std::string yesNo = no + "   " + yes;
			CONSOLE_MANIP::print_text(boxWidth / 2 - yesNo.length() / 2, boxHeight / 2, yesNo);

			//Czyszczynie bufora wejścia, aby po wduszeniu przycisku,
			// jego akcja nie została wielokrotnie wykonana
			CONSOLE_MANIP::clear_console_input_buffer();

			//Sprawdzanie naciśniętych klawiszy
			if (CONSOLE_MANIP::check_arrow("LEFT") && choice) { choice = false; }
			else if (CONSOLE_MANIP::check_arrow("RIGHT") && !choice) { choice = true; }
			else if (CONSOLE_MANIP::check_enter()) { break; }
		}
		if (choice == false) { break; }
	}

	//Kończenie
	WSACleanup();
	CONSOLE_MANIP::cursor_set_pos(boxWidth / 2 - question.length() / 2, boxHeight / 2);
	CONSOLE_MANIP::press_any_key_pause();
	return 0;
}