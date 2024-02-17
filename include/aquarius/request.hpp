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
		virtual ~request() = default;

		DEFINE_VISITABLE_REQUEST()

	public:
		request(request&& other)
			: base_type(std::move(other))
		{}

		request& operator=(request&& other)
		{
			base_type::operator=(std::move(other));

			return *this;
		}

	private:
		request(const request&) = delete;
		request& operator=(const request& other) = delete;
	};
} // namespace aquarius