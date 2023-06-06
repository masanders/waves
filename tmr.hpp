#include <iostream>

#include <thread>
#include <iomanip>
#include <chrono>

class Tmr
{
public:
	Tmr( bool killmsg = true) : km(killmsg)
	{
		start = std::chrono::high_resolution_clock::now();
	}
	~Tmr()
	{
		if (km)
		{
			std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;
			std::cout << std::setw(8);
			std::cout << end.count() << '\n';
		}
	}
	std::chrono::duration<double> present() const
	{
		std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;
		return end;
	}

private:
	bool km;
	std::chrono::high_resolution_clock::time_point start;
};