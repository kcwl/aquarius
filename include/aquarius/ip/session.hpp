#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/timer.hpp>
#include <aquarius/ip/processor.hpp>

using namespace std::chrono_literals;

namespace aquarius
{
	template <auto Tag, typename Adaptor>
	class session : public basic_session<Tag, Adaptor>, public std::enable_shared_from_this<session<Tag, Adaptor>>
	{
	public:
		using base_type = basic_session<Tag, Adaptor>;

		using typename base_type::socket;

		using proc_type = processor<Tag>;

	public:
		session(socket sock, std::chrono::steady_clock::duration timeout = 1s)
			: base_type(std::move(sock))
			, timeout_(timeout)
			, proto_()
			, timer_(this->get_executor())
		{}

	public:
		auto accept() -> awaitable<void>
		{
			co_return co_await this->socket_adaptor_.accept([this, self = this->shared_from_this()] -> awaitable<void>
															{ co_return co_await proto_.accept(self); });
		}

		template<typename Response>
		auto query() -> awaitable<Response>
		{
			co_return co_await proto_.template query<Response>(this->shared_from_this());
		}

	private:
		void start_timer()
		{
			timer_.expires_after(timeout_);
			timer_.async_wait(
				[this, self = this->shared_from_this()](error_code ec)
				{
					if (ec)
					{
						XLOG_ERROR() << "timer error: " << ec.what();
						return;
					}

					proto_.do_timer();

					self->start_timer();
				});
		}

	private:
		std::chrono::steady_clock::duration timeout_;

		proc_type proto_;

		timer<boost::asio::steady_timer> timer_;
	};

	template <auto Tag, typename Adaptor>
	struct is_session_type<session<Tag, Adaptor>> : std::true_type
	{
	};
} // namespace aquarius