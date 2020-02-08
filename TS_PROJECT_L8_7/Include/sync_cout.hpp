#pragma once
#include <iostream>
#include <mutex>


/*
 * Source: https:// stackoverflow.com/questions/14718124/how-to-easily-make-stdcout-thread-safe
 */

class sync_cout
{
private:
	std::mutex mutex_;

public:
	sync_cout() = default;

	template <typename T>
	sync_cout& operator<<(const T& t);

	sync_cout& operator<<(const tm& time);

	sync_cout& operator<<(std::ostream& (*fp)(std::ostream&));
};

template <typename T>
sync_cout& sync_cout::operator<<(const T& t)
{
	mutex_.lock();
	std::cout << t;
	mutex_.unlock();
	return *this;
}

extern sync_cout s_cout;
