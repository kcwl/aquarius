#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_service_pool.hpp>
#include <chrono>
#include <string>

namespace aquarius
{
	template <typename Executor = asio::any_io_executor>
	class basic_module
	{
	public:
		using executor_type = Executor;

	public:
		virtual std::string name() const = 0;

		virtual bool init()
		{
			return true;
		}

		virtual void stop()
		{}

		virtual bool enable()
		{
			return true;
		}

		virtual auto timer(std::chrono::milliseconds) -> asio::awaitable<void>
		{
			co_return;
		}

		virtual auto run() -> asio::awaitable<bool>
		{
			co_return true;
		}

	public:
		void attach(const executor_type& executor)
		{
			executor_ = executor;
		}

	protected:
		executor_type executor_;
	};

	template <typename T>
	class module : public basic_module<>
	{
		using base_type = basic_module<>;

	public:
		using executor_type = typename base_type::executor_type;

	public:
		module() = default;

	public:
		virtual std::string name() const final
		{
			constexpr auto module_name = detail::struct_name<T>();

			return std::string(module_name);
		}

		template <typename R, typename Func>
		auto async_visit(Func&& f) -> asio::awaitable<R>
		{
			co_return co_await asio::co_spawn(
				this->executor_, [func = std::move(f), this] mutable -> asio::awaitable<R>
				{ co_return co_await func(_this()); }, asio::use_awaitable);
		}

	private:
		T* _this()
		{
			return reinterpret_cast<T*>(this);
		}
	};
} // namespace aquarius