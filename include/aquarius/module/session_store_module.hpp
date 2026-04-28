#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/module/module_register.hpp>

namespace aquarius
{
	class session_store_impl
	{
	public:
		virtual ~session_store_impl() = default;

	public:
		virtual auto async_send(flex_buffer buffer) -> asio::awaitable<error_code> = 0;

		virtual auto async_send(flex_buffer_view buffer) -> asio::awaitable<error_code> = 0;

		virtual std::size_t uuid() const = 0;

		virtual void close() = 0;
	};

	template <typename T>
	class session_impl : public session_store_impl
	{
	public:
		session_impl(std::shared_ptr<T> session)
			: session_ptr_(session)
		{}

		virtual ~session_impl() = default;

	public:
		virtual auto async_send(flex_buffer buffer) -> asio::awaitable<error_code> override
		{
			co_return co_await session_ptr_->async_send(std::move(buffer));
		}

		virtual auto async_send(flex_buffer_view buffer) -> asio::awaitable<error_code> override
		{
			co_return co_await session_ptr_->async_send(buffer);
		}

		virtual std::size_t uuid() const override
		{
			return session_ptr_->uuid();
		}

		virtual void close() override
		{
			session_ptr_->close();
		}

	private:
		std::shared_ptr<T> session_ptr_;
	};

	AQUARIUS_MODULE(session_store_module)
	{
	public:
		session_store_module() = default;

	public:
		void attach(std::shared_ptr<session_store_impl> session_ptr)
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(session_ptr->uuid());

			if (iter != sessions_.end())
			{
				return;
			}

			sessions_.insert({ session_ptr->uuid(), session_ptr });
		}

		void shutdown(std::shared_ptr<session_store_impl> session_ptr)
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(session_ptr->uuid());
			if (iter == sessions_.end())
			{
				return;
			}

			iter->second->close();

			sessions_.erase(iter);
		}

		auto broadcast(flex_buffer& buffer) -> asio::awaitable<error_code>
		{
			std::lock_guard lk(mutex_);

			for (auto& session : sessions_)
			{
				flex_buffer_view tmp((char*)buffer.data().data(), buffer.size());

				auto ec = co_await session.second->async_send(tmp);

				if (ec)
				{
					co_return ec;
				}
			}

			buffer.consume(buffer.size());

			co_return error_code{};
		}

		//auto invoke_v(std::size_t uid, flex_buffer_view buffer) -> asio::awaitable<error_code>
		//{
		//	std::lock_guard lk(mutex_);

		//	auto iter = sessions_.find(uid);

		//	if (iter == sessions_.end())
		//	{
		//		co_return error_code();
		//	}

		//	if (!iter->second)
		//	{
		//		co_return error_code();
		//	}

		//	co_return co_await iter->second->async_send(buffer);
		//}

		auto invoke(const std::size_t uid, flex_buffer buffer) -> asio::awaitable<error_code>
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(uid);

			if (iter == sessions_.end())
			{
				co_return error_code();
			}

			if (!iter->second)
			{
				co_return error_code();
			}

			co_return co_await iter->second->async_send(std::move(buffer));
		}

	private:
		std::mutex mutex_;

		std::map<std::size_t, std::shared_ptr<session_store_impl>> sessions_;
	};
} // namespace aquarius