#pragma once
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(channels)

//class test_service : public aquarius::service
//{
//public:
//	test_service() = default;
//
//public:
//	virtual bool init() override
//	{
//		return true;
//	}
//
//	virtual bool config() override
//	{
//		return true;
//	}
//
//	virtual bool run() override
//	{
//		return true;
//	}
//
//	virtual void stop() override
//	{
//		return;
//	}
//
//	virtual bool enable() override
//	{
//		return true;
//	}
//
//	virtual std::string name() override
//	{
//		return "test_service";
//	}
//};
//
//class test_service_1 : public aquarius::service
//{
//public:
//	test_service_1() = default;
//
//public:
//	virtual bool init() override
//	{
//		return false;
//	}
//
//	virtual bool config() override
//	{
//		return true;
//	}
//
//	virtual bool run() override
//	{
//		return true;
//	}
//
//	virtual void stop() override
//	{
//		return;
//	}
//
//	virtual bool enable() override
//	{
//		return true;
//	}
//
//	virtual std::string name() override
//	{
//		return "test_service_1";
//	}
//};
//
//class test_service_2 : public aquarius::service
//{
//public:
//	test_service_2() = default;
//
//public:
//	virtual bool init() override
//	{
//		return true;
//	}
//
//	virtual bool config() override
//	{
//		return false;
//	}
//
//	virtual bool run() override
//	{
//		return true;
//	}
//
//	virtual void stop() override
//	{
//		return;
//	}
//
//	virtual bool enable() override
//	{
//		return true;
//	}
//
//	virtual std::string name() override
//	{
//		return "test_service_2";
//	}
//};
//
//class test_service_3 : public aquarius::service
//{
//public:
//	test_service_3() = default;
//
//public:
//	virtual bool init() override
//	{
//		return true;
//	}
//
//	virtual bool config() override
//	{
//		return true;
//	}
//
//	virtual bool run() override
//	{
//		return false;
//	}
//
//	virtual void stop() override
//	{
//		return;
//	}
//
//	virtual bool enable() override
//	{
//		return true;
//	}
//
//	virtual std::string name() override
//	{
//		return "test_service_3";
//	}
//};
//
//class test_service_4 : public aquarius::service
//{
//public:
//	test_service_4() = default;
//
//public:
//	virtual bool init() override
//	{
//		return true;
//	}
//
//	virtual bool config() override
//	{
//		return true;
//	}
//
//	virtual bool run() override
//	{
//		return true;
//	}
//
//	virtual void stop() override
//	{
//		return;
//	}
//
//	virtual bool enable() override
//	{
//		return false;
//	}
//
//	virtual std::string name() override
//	{
//		return "test_service_4";
//	}
//};
//
//AQUARIUS_SERVICE_REGIST(10000, test_service);
//AQUARIUS_SERVICE_REGIST(10001, test_service_1);
//AQUARIUS_SERVICE_REGIST(10002, test_service_2);
//AQUARIUS_SERVICE_REGIST(10003, test_service_3);
//AQUARIUS_SERVICE_REGIST(10004, test_service_4);

class pub : public aquarius::channel::publisher
{
public:
	pub()
	{
		BOOST_CHECK(this->publish("xxx","xx1"));
	}
};

class pub_not_exist : public aquarius::channel::publisher
{
public:
	pub_not_exist()
	{
		BOOST_CHECK(!this->publish("xxy", "xx1"));
	}
};

class sub : public aquarius::channel::subscriber<sub>
{
public:
	sub()
	{
		this->subscribe("xxx");
	}
};

BOOST_AUTO_TEST_CASE(group)
{
	{
		sub s1{};

		sub s2{};

		pub p{};

		pub_not_exist pp{};

		auto channel = aquarius::channel::default_group::instance().find("xxx");

		BOOST_CHECK(channel);
	}
	
	{
		aquarius::channel::default_group::instance().erase("xxx");

		auto channel = aquarius::channel::default_group::instance().find("xxx");

		BOOST_CHECK(!channel);
	}
}

BOOST_AUTO_TEST_SUITE_END()