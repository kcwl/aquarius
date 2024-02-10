#pragma once
#include <any>
#include <aquarius/defines.hpp>
#include <aquarius/event_callback.hpp>
#include <aquarius/message/message.hpp>
#include <aquarius/resolver.hpp>
#include <deque>
#include <string>
#include <memory>
#include <aquarius/detail/config.hpp>
#include <aquarius/context/context.hpp>

namespace aquarius
{
	class xsession : public event_callback
	{
	public:
		xsession() = default;
		virtual ~xsession() = default;

	public:
		virtual std::size_t uid() = 0;

		virtual bool async_write(flex_buffer_t&& buffer) = 0;

		virtual void close() = 0;

		virtual void update_identify(std::size_t id) = 0;

		virtual std::size_t identify() = 0;

		virtual std::string remote_address() = 0;

		virtual void server_port(int32_t port) = 0;

		virtual int32_t server_port() = 0;

		virtual void attach(std::size_t proto, std::shared_ptr<context> context_ptr) = 0;

		virtual void detach(std::size_t proto) = 0;
	};

	template <typename _Connector>
	class session : public xsession
	{
		friend class context;

	public:
		explicit session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr)
			, identify_(0)
		{}

		virtual ~session() = default;

	public:
		virtual std::size_t uid()
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

		virtual void close() override
		{
			ctxs_.clear();

			if (conn_ptr_)
				conn_ptr_->shut_down();
		}

		virtual void update_identify(std::size_t id) override
		{
			identify_ = id;
		}

		virtual std::size_t identify() override
		{
			return identify_;
		}

		virtual std::string remote_address() override
		{
			if (!conn_ptr_)
				return {};

			return conn_ptr_->remote_address();
		}

		virtual void server_port(int32_t port) override
		{
			server_port_ = port;
		}

		virtual int32_t server_port() override
		{
			return server_port_;
		}

		virtual void attach(std::size_t proto, std::shared_ptr<context> context_ptr) override
		{
			std::lock_guard lk(mutex_);

			auto iter = ctxs_.find(proto);

			if (iter != ctxs_.end())
				return;

			ctxs_.insert({ proto,context_ptr });
		}

		virtual void detach(std::size_t proto) override
		{
			std::lock_guard lk(mutex_);

			ctxs_.erase(proto);
		}

	public:
		virtual void on_accept() final
		{}

		virtual void on_close() final
		{}

		virtual void on_connect() final
		{}

		virtual void on_timeout() final
		{}

	private:
		void erase_context(uint32_t proto)
		{
			ctx_queue_.pop_front();

			auto iter = ctxs_.find(proto);

			if (iter != ctxs_.end())
				return;

			std::shared_ptr<context>().swap(iter->second);
		}

	private:
		std::shared_ptr<_Connector> conn_ptr_;

		std::mutex mutex_;

		std::unordered_map<std::size_t, std::shared_ptr<context>> ctxs_;

		std::deque<std::pair<std::shared_ptr<xmessage>, std::shared_ptr<context>>> ctx_queue_;

		std::size_t identify_;

		int32_t server_port_;
	};
} // namespace aquarius