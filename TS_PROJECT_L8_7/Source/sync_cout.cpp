#include "sync_cout.hpp"

#include <iomanip>

using namespace std;

sync_cout s_cout;

sync_cout& sync_cout::operator<<(const tm& time)
{
	*this << setfill('0') << setw(2) << time.tm_hour << ':' << setfill('0') << setw(2) << time.
		tm_min << ':' << setfill('0') << setw(2) << time.tm_sec;
	return *this;
}

sync_cout& sync_cout::operator<<(ostream&(*fp)(ostream&))
{
	mutex_.lock();
	cout << fp;
	mutex_.unlock();
	return *this;
}
