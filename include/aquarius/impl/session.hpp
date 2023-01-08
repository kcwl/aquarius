#pragma once
#include <aquarius/impl/connect.hpp>
#include <aquarius/impl/io.hpp>
#include <aquarius/impl/router.hpp>

namespace aquarius
{
	namespace impl
	{
		class session : public std::enable_shared_from_this<session>
		{
		public:
			session(std::shared_ptr<connector<socket<>>> conn_ptr)
				: conn_ptr_(conn_ptr)
			{}

			virtual ~session() = default;

			void async_run()
			{
				[[maybe_unused]] auto res = std::async(std::launch::async, &session::process, this->shared_from_this());
			}

		public:
			void process()
			{
				flex_buffer_t& read_buffer = conn_ptr_->get_read_buffer();

				auto id = read(read_buffer);

				auto self = shared_from_this();

				invoke_helper::invoke(id, self, read_buffer);
			}

			template <typename _Message>
			bool write(_Message&& msg, [[maybe_unused]] int time_out)
			{
				conn_ptr_->queue_packet(std::forward<_Message>(msg));

				return true;
			}

			uint32_t read(flex_buffer_t& buffer)
			{
				if (buffer.size() < sizeof(uint32_t))
					return 0;

				uint32_t id{};

				elastic::binary_iarchive ia(buffer);
				ia >> id;

				return id;
			}

			template <typename _Context>
			void attach_context(_Context* context)
			{}

			template <typename _Context>
			_Context* use_context()
			{
				return nullptr;
			}

			virtual int visit(xmessage* req)
			{
				req;
				return 0;
			}

			int close()
			{
				return 0;
			}

		private:
			std::shared_ptr<connector<socket<>>> conn_ptr_;
		};
	} // namespace impl
} // namespace aquarius