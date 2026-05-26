#include <aquarius.hpp>
#include <iostream>

int main()
{
	aquarius::tcp::server srv(3399, 10, "serviced");

	srv.run();

	return 0;
}