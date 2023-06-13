#pragma once
#include <aquarius/header.hpp>
#include <aquarius/message.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class response : public message<tcp_response_header, _Body, Number>
	{
		using base_type = message<tcp_response_header, _Body, Number>;

	public:
		response() = default;
		~response() = default;

		DEFINE_VISITABLE(int)

	public:
		response(const response& other)
		{
			this->header() = other.header();

			this->body().Copy(other.body());
		}

		response(response&& other)
		{
			this->header() = std::move(other.header());

			this->body().Move(other.body());
		}

		response& operator=(response&& other)
		{
			base_type::operator=(std::move(other));

			return *this;
		}
	};

	template <uint32_t Number>
	using null_body_response = response<null_body, Number>;
} // namespace aquarius
