#include "utils.hpp"

#include "console_manip.hpp"

#include <iomanip>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>


using namespace std;

tm utils::get_current_time()
{
	time_t tt;
	time(&tt);
	tm time = tm();
	localtime_s(&time, &tt);
	time.tm_year += 1900;
	time.tm_mon += 1;
	return time;
}

string utils::remove_trailing_zeros(string number_str)
{
	if (number_str.find('.') == string::npos) { return number_str; }

	for (unsigned i = unsigned(number_str.size() - 1); i > 0; i--)
	{
		if (number_str[i] == '0') { number_str.pop_back(); }
		else if (number_str[i] == '.')
		{
			number_str.pop_back();
			break;
		}
		else { break; }
	}
	return number_str;
}

// Source: https://www.techiedelight.com/reverse-lookup-stl-map-cpp/
unsigned utils::reverse_map_lookup(const map<unsigned, string>& m, const string& value)
{
	auto it = find_if(m.begin(), m.end(),
	                  [&value](const pair<unsigned, string>& p)
	                  {
		                  return p.second == value;
	                  });

	if (it == m.end())
	{
		throw out_of_range("value not found");
	}
	return it->first;
}

int utils::rand_int(const int& min, const int& max)
{
	if (max <= min) return min;
	random_device rd;
	mt19937 gen(rd());
	const uniform_int_distribution<int> d(min, max);
	return d(gen);
}

tm utils::string_to_tm(const std::string& str)
{
	tm result{};
	result.tm_mday = stoi(str.substr(0, 2));
	result.tm_mon = stoi(str.substr(3, 2));
	result.tm_year = stoi(str.substr(6, 2));
	result.tm_hour = stoi(str.substr(11, 2));
	result.tm_min = stoi(str.substr(14, 2));
	result.tm_sec = stoi(str.substr(17, 2));
	return result;
}

ostream& operator <<(ostream& os, const tm& time)
{
	os << setfill('0') << setw(2) << time.tm_mday << '.' << setfill('0') << setw(2) << time.tm_mon << '.' <<
		setfill('0') << setw(4) << time.tm_year << '.'
		<< setfill('0') << setw(2) << time.tm_hour << ':' << setfill('0') << setw(2) << time.tm_min << ':' <<
		setfill('0') << setw(2) << time.tm_sec;
	return os;
}

std::string utils::tm_to_string(const tm& time)
{
	stringstream result_stream;
	result_stream << time;
	return result_stream.str();
}
