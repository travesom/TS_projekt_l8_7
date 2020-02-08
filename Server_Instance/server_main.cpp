#include "sync_cout.hpp"
#include "console_manip.hpp"
#include "calculator_server.hpp"

int main()
{
	console_manip::clear_console();

	udp_server server(27272);
	while (true)
	{
		if (!server.start_session()) { continue; }
		if (!server.handle_session()) {
			s_cout << "\nSession interrupted!\n";
		}
	}
}
