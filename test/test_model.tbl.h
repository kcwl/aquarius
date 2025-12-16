#include <aquarius/tbl.hpp>

using namespace aquarius::tbl;

struct test_model
{
	field<int1, key<primary>> id1;
	field<int2, key<primary>> id2;
	field<int3, key<primary>> id3;
	field<int4, key<primary>> id4;
	field<int8, key<primary>> id5;
	field<float, key<primary>> id6;
	field<double, key<primary>> id7;
};