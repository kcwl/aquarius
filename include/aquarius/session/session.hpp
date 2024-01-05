#pragma once
#include <any>
#include <aquarius/defines.hpp>
#include <aquarius/event_callback.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/message/message.hpp>
#include <aquarius/resolver.hpp>
#include <deque>
#include <string>
#include <memory>

namespace aquarius
{
	class xsession : public event_callback
	{
	public:
		xsession() = default;
		virtual ~xsession() = default;

	public:
		virtual std::size_t uid() = 0;

		virtual read_handle_result process(flex_buffer_t& buffer) = 0;

		virtual bool async_write(flex_buffer_t&& buffer) = 0;

		virtual void close() = 0;

		virtual void update_identify(std::size_t id) = 0;

		virtual std::size_t identify() = 0;

		virtual std::string remote_address() = 0;

		virtual void server_port(int32_t port) = 0;

		virtual int32_t server_port() = 0;
	};

	template <typename _Connector>
	class session : public xsession, public std::enable_shared_from_this<session<_Connector>>
	{
		friend class context;

	public:
		explicit session()
			: session(nullptr)
		{}

		session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr)
			, identify_(0)
		{}

	public:
		virtual std::size_t uid()
		{
			return conn_ptr_ ? conn_ptr_->uuid() : 0;
		}

		virtual read_handle_result process(flex_buffer_t& buffer) override
		{
			if (!ctx_queue_.empty())
			{
				auto& pair = ctx_queue_.front();

				return pair.first->accept(buffer, pair.second, this->shared_from_this());
			}

			uint32_t proto{};

			auto result = resolver<tcp>::template from_binay(buffer, proto);

			if (result != read_handle_result::ok)
				return result;

			if (proto == ip_back_proto)
			{
				return read_handle_result::reset_peer;
			}

			if (proto == ip_report_proto)
			{
				return read_handle_result::report;
			}

			auto request_ptr = message_invoke_helper::invoke(proto);

			if (!request_ptr)
				return read_handle_result::unknown_proto;

			std::shared_ptr<context> context_ptr;

			auto iter = ctxs_.find(proto);

			if (iter == ctxs_.end())
			{
				context_ptr = context_invoke_helper::invoke(proto);
			}
			else
			{
				context_ptr = iter->second;
			}

			if (!context_ptr)
				return read_handle_result::unknown_ctx;

			result = request_ptr->accept(buffer, context_ptr, this->shared_from_this());

			if (result != read_handle_result::unknown_error)
			{
				ctx_queue_.push_back({ request_ptr, context_ptr });

				return result;
			}

			erase_context(proto);

			return read_handle_result::ok;
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

		std::unordered_map<uint32_t, std::shared_ptr<context>> ctxs_;

		std::deque<std::pair<std::shared_ptr<xmessage>, std::shared_ptr<context>>> ctx_queue_;

		std::size_t identify_;

		int32_t server_port_;
	};
} // namespace aquarius