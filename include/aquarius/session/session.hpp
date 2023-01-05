#pragma once
#include "../router.hpp"
#include "../io.hpp"
#include <aquarius/proto/message.hpp>
#include <aquarius/session/connect.hpp>

namespace aquarius
{
	namespace session
	{
		template<typename _Conn>
		class session 
			: public std::enable_shared_from_this<session<_Conn>>
		{
		public:
			session(std::shared_ptr<_Conn> conn_ptr)
				: conn_ptr_(conn_ptr)
			{

			}

			virtual ~session() = default;

			void async_run()
			{
				std::async(std::launch::deferred, &session::process, this->shared_from_this());
			}

		public:
			void process()
			{
				if (auto request = read())
				{
					using request_t = decltype(request);
					invoke_helper<request_t>::invoke(request->unique_key(), std::move(request));
				}
			}

			template<typename _Message>
			bool write(_Message* msg, int time_out)
			{

			}

			std::shared_ptr<proto::xmessage> read()
			{
				flex_buffer_t& read_buffer = conn_ptr_->get_read_buffer();

				if (read_buffer.size() < sizeof(uint32_t))
					return nullptr;

				uint32_t id{};

				elastic::binary_iarchive ia(read_buffer);
				ia >> id;

				return invoke_msg_helper<std::shared_ptr<proto::xmessage>>::invoke(id);
			}

			template<typename _Context>
			void attach_context(_Context* context)
			{

			}

			template<typename _Context>
			_Context* use_context()
			{
				return nullptr;
			}

			virtual int visit(proto::xmessage* req)
			{
				req;
				return 0;
			}

		private:


		private:
			std::shared_ptr<_Conn> conn_ptr_;
		};
	}
}