#pragma once
#include <ctime>
#include <map>
#include <string>


class utils
{
public:
	static tm get_current_time();

	static std::string remove_trailing_zeros(std::string number_str);

	static unsigned reverse_map_lookup(const std::map<unsigned, std::string>& m, const std::string& value);

	static int rand_int(const int& min, const int& max);

	static tm string_to_tm(const std::string& str);

	static std::string tm_to_string(const tm& time);
};
