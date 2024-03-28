#pragma once
#include <any>
#include <aquarius/context/context.hpp>
#include <aquarius/core/deadline_timer.hpp>
#include <deque>
#include <memory>
#include <string>

namespace aquarius
{
	template <typename _Connector>
	class session : public basic_session
	{
		friend class basic_context;

	public:
		explicit session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr)
			, mutex_()
			, ctxs_()
			, timer_(conn_ptr->get_executor())
		{
			deadline();
		}

		virtual ~session()
		{
			timer_.cancel();
		}

	public:
		virtual std::size_t uuid() override
		{
			if (conn_ptr_.expired())
				return false;

			auto ptr = conn_ptr_.lock();

			return ptr->uuid();
		}

		virtual bool async_write(flex_buffer_t&& buffer) override
		{
			if (conn_ptr_.expired())
				return false;

			auto ptr = conn_ptr_.lock();

			ptr->async_write(std::forward<flex_buffer_t>(buffer));

			return true;
		}

		virtual void attach(std::size_t proto, std::shared_ptr<basic_context> context_ptr) override
		{
			std::lock_guard lk(mutex_);

			ctxs_.insert({ proto, context_ptr });
		}

		virtual void detach(std::size_t proto) override
		{
			std::lock_guard lk(mutex_);

			ctxs_.erase(proto);
		}

	public:
		void on_timeout()
		{
			std::lock_guard lk(mutex_);

			for (auto& ctx : ctxs_)
			{
				if (ctx.second.expired())
					continue;

				auto ptr = ctx.second.lock();
				
				ptr->on_timeout();
			}
		}

	private:
		void deadline()
		{
			timer_.expires_from_now(timeout_dura);

			timer_.async_wait(
				[this](const asio::error_code& ec)
				{
					if (ec)
					{
						XLOG_WARNING() << "session [" << uuid() << "] occur error: " << ec.message();

						return;
					}

					on_timeout();

					deadline();
				});
		}

	private:
		std::weak_ptr<_Connector> conn_ptr_;

		std::mutex mutex_;

		std::unordered_multimap<std::size_t, std::weak_ptr<basic_context>> ctxs_;

		deadline_timer timer_;
	};
} // namespace aquarius