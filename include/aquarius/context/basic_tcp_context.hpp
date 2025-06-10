#pragma once
#include <aquarius/basic_context.hpp>
#include <aquarius/ip/tcp.hpp>

namespace aquarius
{
	namespace context
	{
		enum class context_kind
		{
			stream,
			tcp_broadcast,
			transfer
		};

		template <auto Mode>
		class basic_tcp_context : public basic_context<ip::tcp, Mode>
		{
			using base_type = basic_context<ip::tcp, Mode>;

		public:
			using base_type::mode;

		public:
			basic_tcp_context(const std::string& name, std::chrono::steady_clock::duration timeout = 30ms)
				: base_type(name, timeout)
			{}
		};
	} // namespace context
} // namespace aquarius