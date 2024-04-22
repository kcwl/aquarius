#pragma once
#include <aquarius/core/elastic.hpp>
#include <aquarius/core/visitable.hpp>
#include <aquarius/message/field.hpp>
#include <aquarius/message/header.hpp>
#include <cstddef>
#include <optional>

namespace aquarius
{
	class basic_context;

	class basic_session;
} // namespace aquarius

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
	class basic_message : public visitable<flex_buffer_t, basic_context, basic_session>,
						  virtual public impl::basic_header
	{
	public:
		basic_message() = default;
		virtual ~basic_message() = default;

	public:
		basic_message(basic_message&& other) = default;

	public:
		AQUARIUS_VISITABLE_BASE()

	public:
		bool from_binary(flex_buffer_t& stream)
		{
			std::size_t length{};

			elastic::from_binary(length, stream);

			stream.consume(static_cast<int>(length));

			return true;
		}
	};

	template <typename _Header, typename _Body, std::size_t N>
	class message : public tcp_header<_Header, N>, public basic_message
	{
		using base_type = tcp_header<_Header, N>;

	public:
		using body_type = _Body;

	public:
		message()
			: body_()
		{}

		virtual ~message() = default;

		message(message&& other) noexcept
			: virtual_base_type(std::move(other))
			, base_type(std::move(other))
			, basic_message(std::move(other))
			, body_(other.body_)
		{
			body_type{}.swap(other.body_);
		}

		message& operator=(message&& other)
		{
			message(std::move(other)).swap(*this);

			return *this;
		}

	public:
		body_type& body() noexcept
		{
			return body_;
		}

		bool from_binary(flex_buffer_t& stream)
		{
			if (!base_type::from_binary(stream))
				return false;

			elastic::from_binary(body_, stream);

			return true;
		}

		bool to_binary(flex_buffer_t& stream)
		{
			base_type::to_binary(stream);

			auto current = stream.size();

			elastic::to_binary(body_, stream);

			this->add_length(stream.size() - current);

			this->complete(stream);

			return true;
		}

	private:
		message(const message& other) = delete;
		message& operator=(const message&) = delete;

		void swap(message& other)
		{
			base_type::swap(other);

			body_.swap(other.body_);
		}

	private:
		body_type body_;
	};
} // namespace aquarius