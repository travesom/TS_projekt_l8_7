#pragma once
#include <iostream>
#include <iomanip>
#include <mutex>

//Bezpieczny dla wielu w�tk�w std::cout
class syncCout {
private:
	std::mutex mutex;

public:
	syncCout(){}

	template<typename T>
	syncCout& operator<<(const T& _t){
		mutex.lock();
		std::cout << _t;
		mutex.unlock();
		return *this;
	}

	syncCout& operator << (const tm& time) {
		std::cout << std::setfill('0') << std::setw(2) << time.tm_hour << ':' << std::setfill('0') << std::setw(2) << time.tm_min << ':' << std::setfill('0') << std::setw(2) << time.tm_sec;
		return *this;
	}

	syncCout& operator << (std::ostream& (*fp)(std::ostream&)){
		mutex.lock();
		std::cout << fp;
		mutex.unlock();
		return *this;
	}
};

//Bezpieczny dla wielu w�tk�w std::cerr
class syncCerr {
private:
	std::mutex mutex;

public:
	syncCerr(){}

	template<typename T>
	syncCerr& operator<<(const T& _t)
	{
		mutex.lock();
		std::cerr << _t;
		mutex.unlock();
		return *this;
	}

	syncCerr& operator<<(std::ostream& (*fp)(std::ostream&))
	{
		mutex.lock();
		std::cerr << fp;
		mutex.unlock();
		return *this;
	}
};

//Statyczne obiekty do u�ywacia zamiast std::cout i std::cerr
static syncCout sync_cout;
static syncCerr sync_cerr;