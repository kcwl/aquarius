#pragma once
#include <aquarius/message/message.hpp>
#include <aquarius/message/tcp_header.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class request : public message<tcp_request_header, _Body, Number>,
					public std::enable_shared_from_this<request<_Body, Number>>
	{
		using base_type = message<tcp_request_header, _Body, Number>;

	public:
		request() = default;
		~request() = default;

		DEFINE_VISITABLE_REQUEST(read_handle_result)

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
	using null_body_request = request<null_body, Number>;
} // namespace aquarius