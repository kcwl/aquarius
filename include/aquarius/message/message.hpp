#pragma once
#include <aquarius/message/basic_header.hpp>
#include <aquarius/serialize.hpp>

namespace aquarius
{
	template <typename _Header, typename _Body, std::size_t N>
	class message : public basic_header<N>
	{
		using base_type = basic_header<N>;

		using header_type = _Header;

	public:
		using body_type = _Body;

	public:
		message()
			: header_()
			, body_()
		{}

		virtual ~message() = default;

		message(message&& other) noexcept
			: base_type(std::move(other))
			, header_(other.header_)
			, body_(other.body_)
		{
			header_type{}.swap(other.header_);

			body_type{}.swap(other.body_);
		}

		message& operator=(message&& other)
		{
			message(std::move(other)).swap(*this);

			return *this;
		}

		message(const message& other) = default;
		message& operator=(const message&) = default;

	public:
		header_type* header() noexcept
		{
			return &header_;
		}

		body_type& body() noexcept
		{
			return body_;
		}

		virtual error_code from_binary(flex_buffer_t& stream, error_code& ec)
		{
			this->set_single(false);

			ec = base_type::from_binary(stream, ec);

			if (ec)
			{
				return ec;
			}

			aquarius::from_binary(header_, stream);

			//aquarius::from_binary(body_, stream);
			body_.from_binary(stream);

			return ec = errc::ok;
		}

		virtual error_code to_binary(flex_buffer_t& stream, error_code& ec)
		{
			this->reset();

			base_type::to_binary(stream, ec);

			auto current = stream.size();

			aquarius::to_binary(header_, stream);

			//aquarius::to_binary(body_, stream);
			body_.to_binary(stream);

			this->add_length(stream.size() - current);

			this->complete(stream, ec);

			return ec = errc::ok;
		}

	private:
		void swap(message& other)
		{
			base_type::swap(other);

			header_.swap(other.header_);

			body_.swap(other.body_);
		}

	private:
		header_type header_;

		body_type body_;
	};
} // namespace aquarius