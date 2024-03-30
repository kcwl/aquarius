#pragma once
#include <aquarius/core/visitable.hpp>
#include <aquarius/core/elastic.hpp>
#include <aquarius/message/field.hpp>
#include <aquarius/message/header.hpp>
#include <cstddef>
#include <optional>

#define DEFINE_VISITABLE_REQUEST()                                                                                     \
	virtual bool accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                 \
						std::shared_ptr<aquarius::basic_session> session_ptr)                                          \
	{                                                                                                                  \
		aquarius::error_code ec{};                                                                                     \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                                                 \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr);                                         \
	}

#define DEFINE_VISITABLE_RESPONSE()                                                                                    \
	virtual bool accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                 \
						std::shared_ptr<aquarius::basic_session> session_ptr)                                          \
	{                                                                                                                  \
		aquarius::error_code ec{};                                                                                     \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                                                 \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr);                                         \
	}

#define DEFINE_VISITABLE()                                                                                             \
	virtual bool accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                 \
						std::shared_ptr<aquarius::basic_session> session_ptr)                                          \
	{                                                                                                                  \
		aquarius::error_code ec{};                                                                                     \
		this->from_binary(buffer, ec);                                                                                 \
		if (ec)                                                                                                        \
			return ec;                                                                                                 \
		return accept_bare_impl(this, ctx, session_ptr);                                                               \
	}

namespace aquarius
{
	class basic_context;

	class basic_session;
}

namespace aquarius
{
	class basic_message : public visitable<flex_buffer_t, basic_context, basic_session>
	{
	public:
		basic_message() = default;
		virtual ~basic_message() = default;

	public:
		DEFINE_VISITABLE()

	public:
		error_code from_binary(flex_buffer_t& stream, error_code& ec)
		{
			std::size_t length{};

			if (!elastic::from_binary(length, stream))
				return ec = system_errc::invalid_stream;

			stream.consume(length);

			return ec = {};
		}

		error_code to_binary(flex_buffer_t&, error_code& ec)
		{
			return ec = system_errc::invalid_message;
		}
	};

	template <typename _Header, typename _Body, std::size_t N>
	class message : public tcp_header<_Header, N>, public basic_message
	{
		using base_type = tcp_header<_Header, N>;

	public:
		using body_type = _Body;

		DEFINE_VISITABLE()

	public:
		message()
			: body_()
		{}

		virtual ~message() = default;

		message(message&& other)
			: base_type(std::forward<message>(other))
			, body_(other.body_)
		{
			message{}.swap(other);
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

			return ec = error_code{};
			;
		}

		error_code to_binary(flex_buffer_t& stream, error_code& ec)
		{
			if (base_type::to_binary(stream, ec) != system_errc::ok)
				return ec;

			if (!elastic::to_binary(body_, stream))
				return ec = system_errc::invalid_stream;

			this->complete(stream, ec);

			return ec;
		}

	private:
		void swap(message& other)
		{
			body_.swap(other.body_);
		}

	private:
		body_type body_;
	};
} // namespace aquarius