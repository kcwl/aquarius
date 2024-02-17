#pragma once
#include <aquarius/message/message.hpp>
#include <aquarius/message/tcp_header.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class response : public message<tcp_response_header, _Body, Number>,
					 public std::enable_shared_from_this<response<_Body, Number>>
	{
		using base_type = message<tcp_response_header, _Body, Number>;

	public:
		response() = default;
		~response() = default;

		DEFINE_VISITABLE_RESPONSE()

	public:
		response(response&& req)
			: base_type(std::move(req))
		{
			
		}

		response& operator=(response&& other)
		{
			base_type::operator=(std::move(other));

			return *this;
		}

	private:
		response(const response&) = delete;
		response& operator=(const response& other) = delete;
	};
} // namespace aquarius
