#pragma once
#include <aquarius/flex_buffer.hpp>
#include <aquarius/message/context.hpp>
#include <aquarius/resolver.hpp>
#include <string>

namespace aquarius
{
	template <typename _Connector>
	class session
	{
	public:
		explicit session()
			: session(nullptr)
		{}

		session(std::shared_ptr<_Connector> conn_ptr)
			: conn_ptr_(conn_ptr)
		{}

	public:
		bool open_session(flex_buffer_t& buffer, std::shared_ptr<_Connector> conn_ptr)
		{
			if (!conn_ptr)
				return false;

			conn_ptr_ = conn_ptr;

			uint32_t proto{};

			auto result = resolver<tcp>::template from_binay(buffer, proto);

			auto request_ptr = message_invoke_helper::invoke(proto_number);

			if (!request_ptr)
				return false;

			std::shared_ptr<context> context_ptr;

			auto iter = ctx_.find(proto);

			if (iter == ctx_.end())
			{
				context_ptr = context_invoke_helper::invoke(proto_number);
			}
			else
			{
				context_ptr = iter->second;
			}

			if (!context_ptr)
				return false;

			if (!request_ptr->accept(buffer, context_ptr))
				return false;

			erase_context(proto);

			return true;
		}

		void close_session()
		{
			ctxs_.clear();
		}

	private:
		void erase_context(uint32_t proto)
		{
			auto iter = ctxs_.find(proto);

			if (iter == ctxs_.end())
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
	};
} // namespace aquarius