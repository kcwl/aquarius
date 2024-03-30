#pragma once
#include <any>
#include <aquarius/connect/impl/session.hpp>
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
		{}

		virtual ~session() = default;

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

	private:
		std::weak_ptr<_Connector> conn_ptr_;

		std::mutex mutex_;

		std::unordered_multimap<std::size_t, std::weak_ptr<basic_context>> ctxs_;
	};
} // namespace aquarius