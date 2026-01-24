#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/processor.hpp>
#include <aquarius/timer.hpp>

using namespace std::chrono_literals;

namespace aquarius
{
	template <auto Tag, typename ProtoTag, typename Adaptor>
	class session : public basic_session<Tag, ProtoTag, Adaptor>, public std::enable_shared_from_this<session<Tag,ProtoTag, Adaptor>>
	{
	public:
		using base_type = basic_session<Tag, ProtoTag, Adaptor>;

		using typename base_type::socket;

		using proc_type = processor<Tag, ProtoTag>;

		using callback_func = std::function<void(std::shared_ptr<session<Tag, ProtoTag, Adaptor>>)>;

	public:
		session(socket sock, std::chrono::steady_clock::duration timeout = 1s)
			: base_type(std::move(sock))
			, proto_()
			, timer_(this->get_executor(), timeout)
		{}

	public:
		auto accept() -> awaitable<void>
		{
			co_return co_await this->socket_adaptor_.accept([this, self = this->shared_from_this()] -> awaitable<void>
															{ co_return co_await proto_.accept(self); });
		}

		template <typename Response>
		auto query(std::size_t seq_number) -> awaitable<Response>
		{
			co_return co_await proto_.template query<Response>(seq_number, this->shared_from_this());
		}

		auto query_buffer(std::size_t seq_number) -> awaitable<flex_buffer>
		{
			co_return co_await proto_.query_buffer(seq_number, this->shared_from_this());
		}

		void set_close_func(const callback_func& f)
		{
			close_func_ = f;
		}

	private:
		void start_timer()
		{
			timer_.async_wait(
				[this, self = this->shared_from_this()](error_code ec)
				{
					if (ec)
					{
						XLOG_ERROR() << "timer error: " << ec.what();
						return;
					}

					proto_.do_timer();
				});
		}

	private:
		proc_type proto_;

		timer<boost::asio::steady_timer> timer_;

		callback_func close_func_;
	};

	template <auto Tag, typename ProtoTag, typename Adaptor>
	struct is_session_type<session<Tag, ProtoTag, Adaptor>> : std::true_type
	{};
} // namespace aquarius