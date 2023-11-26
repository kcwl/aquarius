#pragma once
#include <aquarius/elastic.hpp>

#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(els_tst)

BOOST_AUTO_TEST_CASE(els)
{
	aquarius::flex_buffer_t buffer{};

	xxx::person persons{};

	persons.age = (1);

	persons.name = ("person");

	persons.sex = true;

	persons.money = (1111);

	persons.back_money = (22222);

	persons.crc = (111122222);

	persons.role_data = (std::string{ 'a', 'b' });

	persons.hp = (1.2f);

	persons.mana = (2222.33);

	persons.input_stream = (aquarius::fixed<uint32_t>(5));

	persons.output_stream = (aquarius::fixed<uint64_t>(10));

	aquarius::to_binary(persons, buffer);

	xxx::person per{};

	aquarius::from_binary(per, buffer);

	BOOST_CHECK(per.age == persons.age);
	BOOST_CHECK(per.name == persons.name);
	BOOST_CHECK(per.sex == persons.sex);
	BOOST_CHECK(per.money == persons.money);
	BOOST_CHECK(per.back_money == persons.back_money);
	BOOST_CHECK(per.crc == persons.crc);
	BOOST_CHECK(per.role_data == persons.role_data);
	BOOST_CHECK(per.hp == persons.hp);
	BOOST_CHECK(per.mana == persons.mana);
	BOOST_CHECK(per.input_stream == persons.input_stream);
	BOOST_CHECK(per.output_stream == persons.output_stream);
}

BOOST_AUTO_TEST_SUITE_END()