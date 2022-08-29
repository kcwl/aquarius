#pragma once
#include "../stream.hpp"

namespace aquarius
{
	namespace conn
	{
		struct tcp_session {};

		struct http_session {};


		template<typename _Ty, typename _Conn>
		class basic_session : public _Conn
		{
		public:
			basic_session(boost::asio::io_service& io_service)
				: _Conn(io_service)
			{

			}

			virtual ~basic_session() = default;

			void start()
			{
				this->async_read();
			}

			void on_close()
			{
				return;
			}

			virtual bool read_handle()
			{
				ftstream& read_buffer = this->get_read_buffer();

				if (read_buffer.size() < sizeof(uint32_t))
					return false;

				auto proto_id = read_buffer.get<uint32_t>();

				auto shared_this = _Conn::shared_from_this();

				invoke_helper::invoke(proto_id, std::move(shared_this), read_buffer);

				return true;
			}
		};

		template<typename _Conn>
		class basic_session<http_session, _Conn> : public _Conn
		{
		public:
			basic_session(boost::asio::io_service& io_service)
				: _Conn(io_service)
			{

			}

			virtual ~basic_session() = default;

			void start()
			{
				this->async_read();
			}

			void on_close()
			{
				return;
			}

			virtual bool read_handle()
			{
				return true;
			}
		};
	}
}