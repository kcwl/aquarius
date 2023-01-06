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
				[[maybe_unused]] auto res =
					std::async(std::launch::deferred, &session::process, this->shared_from_this());
			}

		public:
			void process()
			{
				auto [request,id] = read();

				auto self = shared_from_this();

				invoke_helper::invoke(id, std::move(self), std::move(request));
			}

			template <typename _Message>
			bool write(_Message* msg, int time_out)
			{}

			std::pair<std::shared_ptr<xmessage>,uint32_t> read()
			{
				flex_buffer_t& read_buffer = conn_ptr_->get_read_buffer();

				if (read_buffer.size() < sizeof(uint32_t))
					return std::pair<std::shared_ptr<xmessage>, uint32_t>{};

				uint32_t id{};

				elastic::binary_iarchive ia(read_buffer);
				ia >> id;

				return { invoke_msg_helper<std::shared_ptr<xmessage>>::invoke(id), id };
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