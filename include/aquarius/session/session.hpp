#pragma once
#include <any>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/message/context.hpp>
#include <aquarius/resolver.hpp>
#include <deque>
#include <string>

namespace aquarius
{
	class xsession
	{
	public:
		xsession() = default;
		virtual ~xsession() = default;

	public:
		virtual std::size_t uid() = 0;

		virtual read_handle_result process(flex_buffer_t& buffer) = 0;
	};

	template <typename _Connector>
	class session : public std::enable_shared_from_this<session<_Connector>>
	{
		friend class context;

	public:
		explicit session()
			: session(nullptr)
		{}

		session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr)
		{}

	public:
		virtual std::size_t uid()
		{
			return conn_ptr_ ? conn_ptr_->uid() : 0;
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

			auto request_ptr = message_invoke_helper::invoke(proto);

			if (!request_ptr)
				return read_handle_result::unknown_proto;

			std::shared_ptr<context> context_ptr;

			auto iter = ctx_.find(proto);

			if (iter == ctx_.end())
			{
				context_ptr = context_invoke_helper::invoke(proto);
			}
			else
			{
				context_ptr = iter->second;
			}

			if (!context_ptr)
				return read_headle_result::unknown_ctx;

			result = request_ptr->accept(buffer, context_ptr, this->shared_from_this());

			if (result != read_handle_result::unknown_error)
			{
				ctx_queue_.push_back({ request_ptr, context_ptr });

				return result;
			}

			erase_context(proto);

			return read_handle_result::ok;
		}

		void close_session()
		{
			ctxs_.clear();
		}

	private:
		void erase_context(uint32_t proto)
		{
			ctx_queue_.pop_front();

			auto iter = ctxs_.find(proto);

			if (iter != ctxs_.end())
				return;

			std::shared_ptr<context>().swap(iter->second);
		}

		bool send_message(flex_buffer_t&& buffer)
		{
			if (!conn_ptr_)
				return false;

			conn_ptr_->async_write(std::forward<flex_buffer_t>(buffer));

			return true;
		}

	private:
		std::shared_ptr<_Connector> conn_ptr_;

		std::unordered_map<uint32_t, std::shared_ptr<context>> ctxs_;

		std::deque<std::pair<std::shared_ptr<xmessage>, std::shared_ptr<context>>> ctx_queue_;
	};
} // namespace aquarius