#pragma once
#include "../router.hpp"
#include "../io.hpp"

namespace aquarius
{
	namespace session
	{
		struct tcp_session{};
		struct http_session {};

		template<typename _Ty, typename _Conn>
		class session : public _Conn
		{
		public:
			session(boost::asio::io_service& io_service)
				: _Conn(io_service)
			{

			}

			virtual ~session() = default;

			void start()
			{
				this->establish_async_read();
			}

			void on_close()
			{
				return;
			}

			virtual bool read_handle()
			{
				flex_buffer_t& read_buffer = this->get_read_buffer();

				if (read_buffer.size() < sizeof(uint32_t))
					return false;

				int32_t proto_id = 0;

				elastic::binary_iarchive ia(read_buffer);
				ia >> proto_id;

				auto shared_this = _Conn::shared_from_this();

				invoke_helper::invoke(proto_id, std::move(shared_this), read_buffer);

				return true;
			}
		};

		template<typename _Conn>
		class session<http_session, _Conn> : public _Conn
		{
		public:
			session(boost::asio::io_service& io_service)
				: _Conn(io_service)
			{

			}

			virtual ~session() = default;

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