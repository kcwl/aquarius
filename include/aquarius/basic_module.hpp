#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/error_code.hpp>
#include <chrono>
#include <string>

namespace aquarius
{
	class module_base
	{
	public:
		virtual std::string name() const = 0;

		virtual bool init() = 0;

		virtual void stop() = 0;

		virtual bool enable() = 0;

		virtual auto timer(std::chrono::milliseconds) -> asio::awaitable<void> = 0;

		virtual auto run() -> asio::awaitable<bool> = 0;
	};

	template <typename T>
	class basic_module : public module_base
	{
	public:
		basic_module() = default;

	public:
		template <typename R, typename Func>
		auto async_visit(Func&& f) -> asio::awaitable<R>
		{
			co_return co_await std::forward<Func>(f)(_this());
		}

		template <typename R, typename Func>
		auto visit(Func&& f) -> R
		{
			return std::forward<Func>(f)(_this());
		}

	public:
		virtual std::string name() const final
		{
			constexpr auto module_name = detail::struct_name<T>();

			return std::string(module_name);
		}
		virtual bool init() override
		{
			return true;
		}

		virtual void stop() override
		{
			return;
		}

		virtual bool enable() override
		{
			return true;
		}

		virtual auto timer(std::chrono::milliseconds) -> asio::awaitable<void> override
		{
			co_return;
		}

		virtual auto run() -> asio::awaitable<bool> override
		{
			co_return true;
		}

	private:
		T* _this()
		{
			return reinterpret_cast<T*>(this);
		}
	};
} // namespace aquarius