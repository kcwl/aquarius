#pragma once
#include <aquarius/connect.hpp>
#include <aquarius/detail/context.hpp>
#include <aquarius/detail/invoke.hpp>

namespace aquarius
{
	template <typename _SocketType = void>
	class tcp_connect : public connect<tcp_connect<_SocketType>, _SocketType>
	{
	public:
		explicit tcp_connect(boost::asio::io_service& io_service,
							 std::chrono::steady_clock::duration dura = detail::heart_time_interval)
			: connect<tcp_connect<_SocketType>, _SocketType>(io_service, dura)
		{}

	public:
		virtual detail::read_handle_result read_handle_internal()
		{
			auto& read_buffer = this->get_read_buffer();

			uint32_t id = 0;

			boost::archive::binary_iarchive ia(read_buffer);
			ia >> id;

			if (id == 0)
			{
				return detail::read_handle_result::waiting_for_query;
			}

			auto ctx_iter = this->ctxs_.find(id);

			std::shared_ptr<detail::context> ctx_ptr;

			if (ctx_iter == this->ctxs_.end())
			{
				ctx_ptr = detail::context_invoke_helper::invoke(id);

				this->ctxs_.insert({ id, ctx_ptr });
			}
			else
			{
				ctx_ptr = ctx_iter->second;
			}

			auto req_ptr = detail::message_invoke_helpter::invoke(id);

			if (!req_ptr)
			{
				return detail::read_handle_result::error;
			}

			auto res = req_ptr->visit(read_buffer, detail::visit_mode::input);

			if (res != detail::read_handle_result::ok)
			{
				return res;
			}

			auto result = req_ptr->accept(ctx_ptr, this->shared_from_this());

			if (result == 0)
			{
				std::cout << std::format("warning: no ctx for req: {}\n", id);
			}

			return detail::read_handle_result::ok;
		}
	};
} // namespace aquarius