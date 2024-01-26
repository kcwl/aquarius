#pragma once
#include <aquarius/connect/ssl_connect.hpp>

namespace aquarius
{
	template <typename _Protocol>
	class client_connect : public ssl_connect<_Protocol>
	{
	public:
		client_connect(boost::asio::io_service& io_service,
					   std::chrono::steady_clock::duration dura = heart_time_interval)
			: ssl_connect<_Protocol>(io_service, dura)
		{}

	public:
		virtual bool on_read(read_handle_result result) override
		{
			if (result != read_handle_result::reset_peer)
				return true;

			tcp_response_header header{};

			this->buffer().sgetn((uint8_t*)&header, sizeof(header));

			change_connect(boost::asio::ip::address_v4(header.result).to_string(), static_cast<int32_t>(header.reserve));

			return false;
		}

		virtual bool on_resolve() override
		{
			auto& buff = this->buffer();

			buff.consume(sizeof(uint32_t) * 3);

			uint32_t uid{};

			buff.sgetn((uint8_t*)&uid, sizeof(uint32_t));

			buff.consume(-((int)sizeof(uint32_t) * 4));

			return func_invoke(uid, buff);
		}

		template <typename _Func>
		void send_request(std::size_t uid, flex_buffer_t&& resp_buf, _Func&& f)
		{
			funcs_.insert({ uid, std::forward<_Func>(f) });

			async_write(std::move(resp_buf), [] {});
		}

	private:
		void change_connect(const std::string& ip_addr, int32_t port)
		{
			boost::system::error_code ec;
			this->socket().shutdown(boost::asio::socket_base::shutdown_both, ec);

			boost::asio::ip::tcp::resolver resolve(this->socket().get_executor());

			auto endpoints = resolve.resolve(ip_addr, std::to_string(port));

			boost::asio::async_connect(this->socket(), endpoints,
									   [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
									   {
										   if (ec)
											   return;

										   this->start();
									   });
		}

		bool func_invoke(std::size_t uid, flex_buffer_t& buffer)
		{
			auto iter = funcs_.find(uid);

			if (iter == funcs_.end())
				return false;

			iter->second(buffer);

			return true;
		}

	private:
		std::map<std::size_t, std::function<void(aquarius::flex_buffer_t&)>> funcs_;
	};
} // namespace aquarius