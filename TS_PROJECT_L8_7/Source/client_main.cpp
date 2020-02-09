#include "client_gui.hpp"

using namespace std;

int main()
{
	client_gui gui("127.0.0.1", 27272);

	while (true)
	{
		if (gui.start_session()) { gui.main_menu(); }
		if (!gui.retry_session_starting()) { break; }
	}

	gui.finish();
	return 0;
}
