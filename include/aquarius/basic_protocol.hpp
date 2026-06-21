#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/serialize/binary.hpp>
#include <boost/core/empty_value.hpp>
#include <iostream>

namespace aquarius
{
	template <typename Header, typename Body>
	class basic_protocol : private boost::empty_value<Body>
	{
	public:
		using header_type = Header;
		using body_type = Body;

	public:
		basic_protocol()
			: boost::empty_value<Body>()
			, header_()
		{}

		basic_protocol(const basic_protocol& other)
			: header_(other.header_)
		{
			this->get() = other.get();
		}

		basic_protocol(basic_protocol&& other) noexcept
			: header_(std::exchange(other.header_, header_type{}))
		{
			this->get() = std::exchange(other.get(), body_type{});
		}

		basic_protocol& operator=(const basic_protocol& other)
		{
			if (this != std::addressof(other))
			{
				header_ = other.header_;
				this->get() = other.get();
			}

			return *this;
		}

		basic_protocol& operator=(basic_protocol&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				header_ = std::exchange(other.header_, header_type{});
				this->get() = std::exchange(other.get(), body_type{});
			}
			return *this;
		}

		virtual ~basic_protocol() = default;

	public:
		virtual error_code commit(flex_buffer&) = 0;

		virtual error_code consume(flex_buffer&) = 0;

	public:
		header_type& header()
		{
			return header_;
		}
		const header_type& header() const
		{
			return header_;
		}
		body_type& body()
		{
			return this->get();
		}
		const body_type& body() const
		{
			return this->get();
		}

	private:
		header_type header_;
	};
} // namespace aquarius