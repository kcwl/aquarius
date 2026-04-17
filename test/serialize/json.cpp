#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius.hpp>

using namespace aquarius;

struct mock_json
{
	int a = -32;
	uint32_t b = 32;
	int64_t c = 35;
	float d = 1.2f;
	std::vector<char> vec = { 1, 2, 3 };
	fixed32 e;
	std::string f = "hello";
};

bool operator==(const mock_json& left, const mock_json& right)
{
	return left.a == right.a
		&& left.b == right.b
		&& left.c == right.c
		&& left.d == right.d
		&& left.vec == right.vec
		&& left.e.value == right.e.value
		&& left.f == right.f;
}

void tag_invoke(const boost::json::value_from_tag, boost::json::value& jv, const mock_json& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("a", local.a);
	jv_obj.emplace("b", local.b);
	jv_obj.emplace("c", local.c);
	jv_obj.emplace("d", local.d);
	jv_obj.emplace("vec", aquarius::json_value_from_array(local.vec));
	jv_obj.emplace("e", local.e.value);
	jv_obj.emplace("f", local.f);
}

mock_json tag_invoke(const boost::json::value_to_tag<mock_json>&, const boost::json::value& jv)
{
	mock_json result{};
	auto obj = jv.try_as_object();
	if (obj->empty())
		return {};
	result.a = static_cast<int32_t>(obj->at("a").as_int64());
	result.b = static_cast<uint32_t>(obj->at("b").as_int64());
	result.c = static_cast<int64_t>(obj->at("c").as_int64());
	result.d = static_cast<float>(obj->at("d").as_double());
	result.vec = aquarius::json_value_to_array(obj->at("vec"));
	result.e.value = static_cast<uint32_t>(obj->at("e").as_int64());
	result.f = obj->at("f").as_string();
	return result;
}

BOOST_AUTO_TEST_SUITE(ut_json)

BOOST_AUTO_TEST_CASE(serialize_and_deserialize)
{
	mock_json bin{};

	flex_buffer buffer{};
	json_parse jp{};
	jp.to_datas(bin, buffer);

	mock_json bin1 = jp.from_datas<mock_json>(buffer);

	auto result = (bin == bin1);

	BOOST_TEST(result);

	buffer.pubseekpos(1,std::ios::in);

	BOOST_CHECK_THROW(jp.from_datas<mock_json>(buffer), std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()