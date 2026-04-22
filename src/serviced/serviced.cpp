#include "cmd_register.h"
#include <aquarius/ip/tcp.hpp>
#include <iostream>

using namespace aquarius;
using namespace aquarius::serviced;

int main()
{
	cmd_register();

	tcp_server srv(3399, 10, "serviced");

	srv.run();

	return 0;
}