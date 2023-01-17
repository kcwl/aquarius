#pragma once
#include <aquarius/detail/type_traits.hpp>
#include <aquarius/impl/connect.hpp>
#include <aquarius/impl/invoke.hpp>

namespace aquarius
{
	namespace impl
	{
		class context;

		class session : public std::enable_shared_from_this<session>
		{
		public:
			session(std::shared_ptr<connector> conn_ptr)
				: conn_ptr_(conn_ptr)
			{}

		public:
			template <typename _Message>
			bool write(_Message&& resp, std::chrono::steady_clock::duration timeout)
			{
				flex_buffer_t fs;

				std::forward<_Message>(resp).visit(fs, visit_mode::output);

				conn_ptr_->write(std::move(fs), timeout);

				return true;
			}

			read_handle_result read()
			{
				flex_buffer_t& read_buffer = conn_ptr_->get_read_buffer();

				uint32_t id = 0;

				elastic::binary_iarchive ia(read_buffer);
				ia >> id;

				if (id == 0)
				{
					return read_handle_result::waiting_for_query;
				}

				auto ctx = use_context(id);

				if (!ctx)
				{
					auto self = shared_from_this();

					ctx = context_invoke_helper::invoke(id);

					if (!ctx)
					{
						ctx = std::make_shared<context>("", 0s);
					}

					attach_context(id, ctx);
				}

				auto req_ptr = message_invoke_helpter::invoke(id);

				if (!req_ptr)
				{
					return read_handle_result::error;
				}

				auto res = req_ptr->visit(read_buffer, visit_mode::input);

				if (res != read_handle_result::ok)
				{
					return res;
				}

				auto result = req_ptr->accept(ctx, shared_from_this());

				if (result == 0)
				{
					std::cout << "warning: no ctx for req\n";
				}

				return read_handle_result::ok;
			}

			std::shared_ptr<context> use_context(uint32_t id)
			{
				auto iter = ctxs_.find(id);

				if (iter == ctxs_.end())
					return nullptr;

				return iter->second;
			}

			void attach_context(uint32_t id, std::shared_ptr<context> ctx_ptr)
			{
				auto iter = ctxs_.find(id);
				if (iter != ctxs_.end())
					return;

				ctxs_.insert({ id, ctx_ptr });
			}

			void close()
			{
				return conn_ptr_->shut_down();
			}

		private:
			std::shared_ptr<connector> conn_ptr_;

			std::map<uint32_t, std::shared_ptr<context>> ctxs_;
		};
	} // namespace impl
} // namespace aquarius