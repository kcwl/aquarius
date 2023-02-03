#pragma once
#include <aquarius/impl/invoke.hpp>
#include <memory>

namespace aquarius
{
	namespace impl
	{
		struct dispatch
		{
			template <typename _Connector>
			static read_handle_result on_connected(std::shared_ptr<_Connector> conn_ptr)
			{
				auto& read_buffer = conn_ptr->get_read_buffer();

				uint32_t id = 0;

				elastic::binary_iarchive ia(read_buffer);
				ia >> id;

				if (id == 0)
				{
					return read_handle_result::waiting_for_query;
				}

				auto ctx_iter = conn_ptr->ctxs_.find(id);

				std::shared_ptr<context> ctx_ptr;

				if (ctx_iter == conn_ptr->ctxs_.end())
				{
					ctx_ptr = context_invoke_helper::invoke(id);

					conn_ptr->ctxs_.insert({ id, ctx_ptr });
				}
				else
				{
					ctx_ptr = ctx_iter->second;
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

				auto result = req_ptr->accept(ctx_ptr, conn_ptr);

				if (result == 0)
				{
					std::cout << "warning: no ctx for req\n";
				}

				return read_handle_result::ok;
			}

			static void on_error()
			{}

			static void on_timeout()
			{}

			static void on_session_closed()
			{}
		};
	} // namespace impl
} // namespace aquarius