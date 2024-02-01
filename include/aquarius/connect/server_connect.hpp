#pragma once
#include <aquarius/connect/ssl_connect.hpp>

namespace aquarius
{
	template <typename _Protocol, std::size_t Identify>
	class server_connect : public ssl_connect<_Protocol, true>
	{
	public:
		constexpr static std::size_t IDENTIFY = Identify;

	public:
		server_connect(boost::asio::io_service& io_service,
					   std::chrono::steady_clock::duration dura = heart_time_interval)
			: ssl_connect<_Protocol, true>(io_service, dura)
		{}

	public:
		virtual bool on_read(read_handle_result result) override
		{
			if (result != read_handle_result::report)
				return true;

			tcp_request_header header{};

			this->buffer().sgetn((uint8_t*)&header, sizeof(header));

			report_session(this->uuid(), header.session_id);

			return true;
		}

		virtual bool on_resolve() override
		{
			return false;
		}
	};
} // namespace aquarius