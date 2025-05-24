#pragma once
#include <aquarius/basic_context.hpp>

namespace aquarius
{
	template <typename Request, typename Response>
	class server_context : public basic_context<Request>
	{
		using base_type = basic_context<Request>;

	public:
		server_context(const std::string& name, const std::chrono::milliseconds& timeout = 100ms)
			: base_type(name, timeout)
		{}

	public:
		template <typename Session>
		int visit(std::shared_ptr<Request> msg, std::shared_ptr<Session> session_ptr)
		{
			auto result = base_type::visit(msg, session_ptr);

			send_response(result, session_ptr);

			return result;
		}

	public:
		Response& response()
		{
			return response_;
		}

	protected:
		template <typename Session>
		void send_response(int result, std::shared_ptr<Session> session_ptr)
		{
			if (!session_ptr)
				return;

			auto fs = make_response(result);

			boost::asio::co_spawn(session_ptr->get_executor(), session_ptr->send_packet(Response::proto, std::move(fs)),
								  boost::asio::detached);
		}

		flex_buffer make_response(int /*result*/)
		{
			// response_.set_result(result);
			flex_buffer fs{};
			response_.to_binary(fs);
			return fs;
		}

	protected:
		Response response_;
	};
} // namespace aquarius