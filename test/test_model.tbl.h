#include <aquarius/tbl.hpp>

struct test_model
{
	constexpr static auto language = "utf8";
	field<int1, key<primary>> id;
	field<int2, key<primary>> id;
	field<int3, key<primary>> id;
	field<int4, key<primary>> id;
	field<int8, key<primary>> id;
	field<float, key<primary>> id;
	field<double, key<primary>> id;
	field<string, key<primary>> id;
	field<datetime, key<primary>> id;
};