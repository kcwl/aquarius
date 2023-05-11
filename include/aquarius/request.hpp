#pragma once
#include <aquarius/tcp/header.hpp>
#include <aquarius/tcp/message.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class request : public tcp::message<tcp::tcp_request_header, _Body, Number>
	{
		using base_type = tcp::message<tcp::tcp_request_header, _Body, Number>;
	public:
		request() = default;
		~request() = default;

		DEFINE_VISITABLE(int)

	public:
		request(const request& other)
		{
			this->header() = other.header();

			this->body().Copy(other.body());
		}

		request(request&& other)
		{
			this->header() = std::move(other.header());

			this->body().Move(other.body());
		}

		request& operator=(request&& other)
		{
			base_type::operator=(std::move(other));

			return *this;
		}
	};

	template <uint32_t Number>
	using null_body_request = request<tcp::null_body, Number>;
} // namespace aquarius