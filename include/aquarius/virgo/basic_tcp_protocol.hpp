#pragma once
#include <aquarius/basic_protocol.hpp>
#include <aquarius/virgo/error.hpp>

namespace aquarius
{
	template <bool Request, detail::string_literal Router, typename Header, typename Body>
	class basic_tcp_protocol : public basic_protocol<Header, Body>
	{
	public:
		using base_type = basic_protocol<Header, Body>;

		using typename base_type::header_type;

		using typename base_type::body_type;

		constexpr static auto has_request = Request;

		constexpr static auto this_router = detail::bind_param<Router>::value;

	public:
		basic_tcp_protocol() = default;

		virtual ~basic_tcp_protocol() = default;

		basic_tcp_protocol(const basic_tcp_protocol& other) = default;

		basic_tcp_protocol(basic_tcp_protocol&& other) noexcept = default;

		basic_tcp_protocol& operator=(const basic_tcp_protocol& other) = default;

		basic_tcp_protocol& operator=(basic_tcp_protocol&& other) noexcept = default;

	public:
		virtual error_code commit(flex_buffer& buffer) override
		{
			error_code ec{};

			try
			{
				binary_parse{}.to_datas(this_router, buffer);

				this->header().serialize(buffer);

				this->body().serialize(buffer);
			}
			catch (...)
			{
				ec = ip::error::commit;
			}

			return ec;
		}

		virtual error_code consume(flex_buffer& buffer) override
		{
			error_code ec{};

			try
			{
				this->header().deserialize(buffer);

				this->body().deserialize(buffer);
			}
			catch (...)
			{
				ec = ip::error::consume;
			}

			return ec;
		}
	};

	template <detail::string_literal Router, typename Header, typename Body>
	class basic_tcp_protocol<false, Router, Header, Body> : public basic_protocol<Header, Body>
	{
	public:
		using base_type = basic_protocol<Header, Body>;

		using typename base_type::header_type;

		using typename base_type::body_type;

		using result_t = int32_t;

		constexpr static auto has_request = false;

		constexpr static auto this_router = detail::bind_param<Router>::value;

	public:
		basic_tcp_protocol()
			: base_type()
			, result_()
			, parse_()
		{}

		virtual ~basic_tcp_protocol() = default;

		basic_tcp_protocol(basic_tcp_protocol&& other) noexcept
			: base_type(std::move(other))
			, result_(std::exchange(other.result_, 0))
			, parse_(std::exchange(other.parse_, {}))
		{}

		basic_tcp_protocol& operator=(basic_tcp_protocol&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				base_type::operator=(std::move(other));
				result_ = std::exchange(other.result_, 0);
				parse_ = std::exchange(other.parse_, {});
			}

			return *this;
		}

	public:
		void result(result_t v)
		{
			result_ = v;
		}

		result_t result() const
		{
			return result_;
		}

		result_t& result()
		{
			return result_;
		}

	public:
		virtual error_code commit(flex_buffer& buffer) override
		{
			error_code ec{};

			try
			{
				binary_parse{}.to_datas(this_router, buffer);

				parse_.to_datas<result_t>(this->result(), buffer);

				this->header().serialize(buffer);

				this->body().serialize(buffer);
			}
			catch (...)
			{
				ec = ip::error::commit;
			}

			return ec;
		}

		virtual error_code consume(flex_buffer& buffer) override
		{
			error_code ec{};

			try
			{
				this->result() = parse_.from_datas<result_t>(buffer);

				this->header().deserialize(buffer);

				this->body().deserialize(buffer);
			}
			catch (...)
			{
				ec = ip::error::consume;
			}

			return ec;
		}

	private:
		result_t result_;

		binary_parse parse_;
	};
} // namespace aquarius