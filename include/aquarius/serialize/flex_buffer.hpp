#pragma once
#include <aquarius/asio.hpp>

namespace aquarius
{
	using flex_buffer = streambuf;

	class auto_consume
	{
	public:
		auto_consume(flex_buffer& buffer)
			: buffer_(buffer)
		{}

		~auto_consume()
		{
			buffer_.consume(1);
		}

	private:
		flex_buffer& buffer_;
	};

	class auto_commit
	{
	public:
		auto_commit(flex_buffer& buffer)
			: buffer_(buffer)
		{}

		~auto_commit()
		{
			buffer_.consume(1);
		}

	private:
		flex_buffer& buffer_;
	};
} // namespace aquarius