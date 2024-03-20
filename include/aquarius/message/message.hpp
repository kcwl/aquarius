#pragma once
#include <aquarius/message/field.hpp>
#include <aquarius/message/header.hpp>
#include <aquarius/message/impl/message.hpp>
#include <cstddef>
#include <optional>

namespace aquarius
{
	template <typename _Header, typename _Body, std::size_t N>
	class message : public tcp_header<_Header, N> , public impl::basic_message
	{
		using base_type = tcp_header<_Header, N>;

	public:
		using body_type =  _Body;

		DEFINE_VISITABLE()

	public:
		message()
			: body_()
		{
		}

		virtual ~message() = default;

		message(message&& other)
			: body_(std::move(other.body_))
		{
			
		}

		message& operator=(message&& other)
		{
			message(std::move(other)).swap(*this);

			return *this;
		}

	private:
		message(const message& other) = delete;
		message& operator=(const message&) = delete;

	public:
		body_type& body() noexcept
		{
			return body_;
		}

		error_code from_binary(flex_buffer_t& stream, error_code& ec)
		{
			if (base_type::from_binary(stream, ec) != system_errc::ok)
				return ec;

			if (!elastic::from_binary(body_, stream))
				return ec = system_errc::invalid_stream;
			
			return ec = error_code{};;
		}

		error_code to_binary(flex_buffer_t& stream, error_code& ec)
		{
			if (!base_type::to_binary(stream, ec))
				return ec;

			if (!elastic::to_binary(body_, stream))
				return ec = system_errc::invalid_stream;

			this->complete(stream, ec);
			
			return ec;
		}

	private:
		void swap(message& other)
		{
			base_type::swap(other);

			body_.swap(other.body_);
		}

	private:
		body_type body_;
	};
} // namespace aquarius