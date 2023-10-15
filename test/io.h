#pragma once
#include <aquarius/flex_buffer.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(io)

BOOST_AUTO_TEST_CASE(streambuf)
{
	{
		// default construction
		aquarius::flex_buffer_t buffer{};

		BOOST_CHECK_EQUAL(static_cast<std::size_t>(std::distance(buffer.begin(), buffer.end())),
						  aquarius::detail::capacity);

		const aquarius::flex_buffer_t const_buffer{};

		BOOST_CHECK_EQUAL(static_cast<std::size_t>(std::distance(const_buffer.begin(), const_buffer.end())),
						  aquarius::detail::capacity);
	}

	{
		// capacity construction
		aquarius::flex_buffer_t buffer(1024);

		BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), 1024);
	}

	{
		// span construction
		char a[] = { 1, 2, 3, 4, 5 };

		std::span sp(a);

		aquarius::flex_buffer_t buffer(sp);

		BOOST_CHECK_EQUAL(buffer.size(), sp.size());

		auto* data = buffer.data();

		for (int i = 0; i < 5; ++i)
		{
			BOOST_CHECK_EQUAL(data[i], a[i]);
		}
	}

	{
		// move construction
		aquarius::flex_buffer_t buffer1{};
		buffer1.sputc('a');

		aquarius::flex_buffer_t buffer2(std::move(buffer1));

		BOOST_CHECK(buffer1.data() == nullptr);

		BOOST_CHECK_EQUAL(buffer2.size(), 1);

		BOOST_CHECK_EQUAL(buffer2.data(), "a");
	}

	{
		// raw poiter construction
		const char* buff = "hello";

		aquarius::flex_buffer_t buffer(buff, 6);

		BOOST_CHECK_EQUAL(buffer.size(), 6);

		for (int i = 0; i < 6; ++i)
		{
			BOOST_CHECK_EQUAL(buff[i], buffer.data()[i]);
		}
	}

	{
		// member active()
		aquarius::flex_buffer_t buf("hello", 6);

		BOOST_CHECK_EQUAL(buf.active(), aquarius::detail::capacity - 6);

		const aquarius::flex_buffer_t const_buf("hello", 6);

		BOOST_CHECK_EQUAL(const_buf.active(), aquarius::detail::capacity - 6);
	}

	{
		// member rdata() and wdata()
		aquarius::flex_buffer_t buf("hello", 6);
		BOOST_CHECK_EQUAL(buf.rdata() - buf.data(), 6);
		BOOST_CHECK_EQUAL(buf.wdata() - buf.data(), 0);

		buf.consume(6);
		BOOST_CHECK_EQUAL(buf.wdata() - buf.data(), 6);

		const aquarius::flex_buffer_t const_buf("hello", 6);
		BOOST_CHECK_EQUAL(const_buf.rdata() - const_buf.data(), 6);
		BOOST_CHECK_EQUAL(const_buf.wdata() - const_buf.data(), 0);
	}

	{
		// member commit() and consume()
		aquarius::flex_buffer_t buffer{};

		buffer.commit(1);

		buffer.consume(2);

		BOOST_CHECK_EQUAL(buffer.wdata() - buffer.data(), 1);

		buffer.consume(-2);

		BOOST_CHECK_EQUAL(buffer.wdata() - buffer.data(), 0);

		buffer.commit(2);

		buffer.consume(-1);

		BOOST_CHECK_EQUAL(buffer.wdata() - buffer.data(), 0);
	}

	{
		// member normalize()
		aquarius::flex_buffer_t buffer{};

		buffer.commit(10);
		buffer.consume(6);

		buffer.normalize();

		BOOST_CHECK_EQUAL(buffer.size(), 4);
		BOOST_CHECK_EQUAL(buffer.active(), aquarius::detail::capacity - 4);

		aquarius::flex_buffer_t buffer1{};
		buffer1.normalize();
		BOOST_CHECK_EQUAL(buffer1.size(), 0);
		BOOST_CHECK_EQUAL(buffer1.active(), aquarius::detail::capacity);
	}

	{
		// member ensure()
		aquarius::flex_buffer_t buffer{};

		buffer.commit(4070);

		auto cur_size = buffer.max_size();

		buffer.ensure();

		BOOST_CHECK_EQUAL(buffer.max_size(), cur_size * 2);

		aquarius::flex_buffer_t buffer1{};
		buffer1.ensure();

		BOOST_CHECK_EQUAL(buffer1.max_size(), buffer1.active());
	}

	{
		// member clear()
		aquarius::flex_buffer_t buffer{};

		buffer.clear();

		BOOST_CHECK_EQUAL(buffer.size(), 0);
		BOOST_CHECK_EQUAL(buffer.active(), aquarius::detail::capacity);
	}

	{
		// member resize()
		aquarius::flex_buffer_t buffer{};

		buffer.resize(1024);

		BOOST_CHECK_EQUAL(buffer.max_size(), 4096);
	}

	{
		// member size()
		const aquarius::flex_buffer_t buffer{};
		BOOST_CHECK_EQUAL(buffer.size(), 0);
	}

	{
		// base member spetc()  spetn() sgetc() sgetn()
		aquarius::flex_buffer_t buffer{};

		auto r = buffer.sgetc();

		buffer.sputc('a');

		BOOST_CHECK_EQUAL(buffer.size(), 1);

		buffer.sputn("hello", 6);

		BOOST_CHECK_EQUAL(buffer.size(), 7);

		r = buffer.sgetc();

		BOOST_CHECK_EQUAL(buffer.size(), 6);
		BOOST_CHECK_EQUAL(r, std::char_traits<char>::to_int_type('a'));

		char* h = new char[6];

		buffer.sgetn(h, 6);

		BOOST_CHECK_EQUAL(buffer.size(), 0);

		delete h;
	}

	{
		//base member pubseekoff() pubseekpos()
		aquarius::flex_buffer_t buffer{};
		buffer.sputn("ahelwo", 6);

		buffer.sgetc();

		buffer.pubseekoff(1, std::ios::cur, std::ios::in);

		BOOST_CHECK_EQUAL(buffer.size(), 4);

		buffer.pubseekpos(3, std::ios::in);

		BOOST_CHECK_EQUAL(buffer.size(), 3);

		buffer.pubseekpos(3, std::ios::out);

		BOOST_CHECK_EQUAL(*buffer.wdata(), 'l');

		buffer.pubseekoff(2, std::ios::beg, std::ios::out);

		BOOST_CHECK_EQUAL(*buffer.wdata(), 'l');

		buffer.pubseekoff(2, std::ios::end, std::ios::in);

		BOOST_CHECK_EQUAL(*buffer.rdata(), 'a');

		buffer.pubseekoff(2, std::ios::end, std::ios::out);

		BOOST_CHECK_EQUAL(*buffer.wdata(), 'l');

		BOOST_TEST(buffer.pubseekpos(4096, std::ios::in));

		BOOST_TEST(buffer.pubseekoff(1, 4, 7));

		BOOST_CHECK_EQUAL(buffer.pubseekoff(1, std::ios::cur, std::ios::out), -1);
	}
}

BOOST_AUTO_TEST_SUITE_END()