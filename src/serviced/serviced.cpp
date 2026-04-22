#include <aquarius/ip/tcp.hpp>
#include <iostream>

int main()
{
	aquarius::tcp_server srv(3399, 10, "serviced");

	srv.run();

	return 0;
}