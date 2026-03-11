#include "command_interface.h"
#include "cpp_generator.h"

using namespace aquarius::lazytool;

int main(int argc, char** argv)
{
	command_interface cli{};

	cpp_generator cpp{};
	cli.regist("--cpp_out", &cpp);

	return cli.run(argc, argv);
}