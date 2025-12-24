#include <aquarius/tbl.hpp>

using namespace aquarius::tbl;

struct test_model
{
	field<int1, key<primary>> id1;
	field<int2> id2;
	field<int3> id3;
	field<int4> id4;
	field<int8> id5;
	field<floatable> id6;
	field<doublable> id7;
};