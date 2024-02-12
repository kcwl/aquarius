#pragma once
#include <aquarius/connect/ssl_connect.hpp>

namespace aquarius
{
	template <typename _Protocol>
	class client_connect : public connect<_Protocol, false>
	{
	public:
		client_connect(boost::asio::io_service& io_service, boost::asio::ssl::basic_context& basic_context,
					   std::chrono::steady_clock::duration dura = heart_time_interval)
			: connect<_Protocol, false>(io_service, basic_context, dura)
		{}

	public:
		virtual bool on_resolve() override
		{
			auto& buff = this->buffer();

			buff.consume(sizeof(uint32_t) + sizeof(uint64_t) * 2);

			uint64_t uid{};

			buff.sgetn((uint8_t*)&uid, sizeof(uint64_t));

			buff.consume(-static_cast<int>(sizeof(uint32_t) + sizeof(uint64_t) * 3));

			return func_invoke(uid, buff);
		}

		template <typename _Func>
		void send_request(std::size_t uid, flex_buffer_t&& resp_buf, _Func&& f)
		{
			funcs_.insert({ uid, std::forward<_Func>(f) });

			async_write(std::move(resp_buf), [] {});
		}

		void set_verify_mode(boost::asio::ssl::verify_mode v)
		{
			this->ssl_socket().set_verify_mode(v);
		}

	private:
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