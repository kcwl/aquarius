#pragma once
#include <any>
#include <aquarius/defines.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/event.hpp>
#include <aquarius/message/message.hpp>
#include <aquarius/resolver.hpp>
#include <deque>
#include <memory>
#include <string>

namespace aquarius
{
	class xsession : public detail::event_call
	{
	public:
		xsession() = default;
		virtual ~xsession() = default;

	public:
		virtual std::size_t uuid() = 0;

		virtual bool async_write(flex_buffer_t&& buffer) = 0;

		virtual void attach(std::size_t proto, std::shared_ptr<basic_context> context_ptr) = 0;

		virtual void detach(std::size_t proto) = 0;
	};

	template <typename _Connector>
	class session : public xsession
	{
		friend class basic_context;

	public:
		explicit session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr)
			, mutex_()
			, ctxs_()
		{}

		virtual ~session() = default;

	public:
		virtual std::size_t uuid() override
		{
			return conn_ptr_ ? conn_ptr_->uuid() : 0;
		}

		virtual bool async_write(flex_buffer_t&& buffer) override
		{
			if (!conn_ptr_)
				return false;

			conn_ptr_->async_write(std::forward<flex_buffer_t>(buffer), [] {});

			return true;
		}

		virtual void attach(std::size_t proto, std::shared_ptr<basic_context> context_ptr) override
		{
			std::lock_guard lk(mutex_);

			auto iter = ctxs_.find(proto);

			if (iter != ctxs_.end())
				return;

			ctxs_.insert({ proto, context_ptr });
		}

		virtual void detach(std::size_t proto) override
		{
			std::lock_guard lk(mutex_);

			ctxs_.erase(proto);
		}

	public:
		virtual void on_accept() final
		{
			std::lock_guard lk(mutex_);

			for (auto& ctx : ctxs_)
			{
				ctx.second->on_accept();
			}
		}

		virtual void on_close() final
		{
			std::lock_guard lk(mutex_);

			if (ctxs_.empty())
				return;

			for (auto& ctx : ctxs_)
			{
				ctx.second->on_close();
			}
		}

		virtual void on_timeout() final
		{
			std::lock_guard lk(mutex_);

			for (auto& ctx : ctxs_)
			{
				ctx.second->on_timeout();
			}
		}

	private:
		std::shared_ptr<_Connector> conn_ptr_;

		std::mutex mutex_;

		std::unordered_map<std::size_t, std::shared_ptr<basic_context>> ctxs_;
	};
} // namespace aquarius