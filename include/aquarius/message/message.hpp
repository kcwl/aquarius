#pragma once
#include <aquarius/core/elastic.hpp>
#include <aquarius/core/visitable.hpp>
#include <aquarius/message/field.hpp>
#include <aquarius/message/header.hpp>
#include <cstddef>
#include <optional>

#define AQUARIUS_VISITABLE_MESSAGE()                                                                                   \
	virtual bool accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                 \
						std::shared_ptr<aquarius::basic_session> session_ptr)                                          \
	{                                                                                                                  \
		auto resullt = this -> from_binary(buffer);                                                                    \
		if (!resullt)                                                                                                  \
			return resullt;                                                                                            \
		return accept_shared_impl(this->shared_from_this(), ctx, session_ptr);                                         \
	}

#define AQUARIUS_VISITABLE_BASE()                                                                                      \
	virtual bool accept(aquarius::flex_buffer_t& buffer, std::shared_ptr<aquarius::basic_context> ctx,                 \
						std::shared_ptr<aquarius::basic_session> session_ptr)                                          \
	{                                                                                                                  \
		auto resullt = this -> from_binary(buffer);                                                                    \
		if (!resullt)                                                                                                  \
			return resullt;                                                                                            \
		return accept_bare_impl(this, ctx, session_ptr);                                                               \
	}

namespace aquarius
{
	class basic_context;

	class basic_session;
} // namespace aquarius

namespace aquarius
{
	class basic_message : public visitable<flex_buffer_t, basic_context, basic_session>, public impl::basic_header
	{
	public:
		basic_message() = default;
		virtual ~basic_message() = default;

	public:
		AQUARIUS_VISITABLE_BASE()

	public:
		bool from_binary(flex_buffer_t& stream)
		{
			std::size_t length{};

			if (!elastic::from_binary(length, stream))
				return false;

			stream.consume(length);

			return true;
		}

		bool to_binary(flex_buffer_t&)
		{
			return true;
		}
	};

	template <typename _Header, typename _Body, std::size_t N>
	class message : virtual public tcp_header<_Header, N>, virtual public basic_message
	{
		using base_type = tcp_header<_Header, N>;

	public:
		using body_type = _Body;

		AQUARIUS_VISITABLE_BASE()

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

		bool from_binary(flex_buffer_t& stream)
		{
			if (!base_type::from_binary(stream))
				return false;

			if (!elastic::from_binary(body_, stream))
				return false;

			return true;
			;
		}

		bool to_binary(flex_buffer_t& stream)
		{
			if (!base_type::to_binary(stream))
				return false;

			if (!elastic::to_binary(body_, stream))
				return false;

			this->complete(stream);

			return true;
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