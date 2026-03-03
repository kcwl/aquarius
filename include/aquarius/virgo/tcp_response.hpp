#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Body>
		class tcp_response : public basic_tcp_protocol<false, tcp_response_header, Body>
		{
		public:
			using base = basic_tcp_protocol<false, tcp_response_header, Body>;

			using base::has_request;

		public:
			tcp_response() = default;

			virtual ~tcp_response() = default;

			tcp_response(const tcp_response& other) = default;

			tcp_response& operator=(const tcp_response& other) = default;

			tcp_response(tcp_response&& other) noexcept = default;

			tcp_response& operator=(tcp_response&& other) noexcept = default;

		public:
			bool operator==(const tcp_response& other) const
			{
				return base::operator==(other);
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				return base::operator<<(os);
			}

		public:
			int result() const
			{
				return result_;
			}

			int& result()
			{
				return result_;
			}

			void result(int r)
			{
				result_ = r;
			}

		private:
			int result_;
		};

		template <typename Body>
		std::ostream& operator<<(std::ostream& os, const tcp_response<Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <typename Body>
	struct is_message_type<virgo::tcp_response<Body>> : std::true_type
	{};
} // namespace aquarius