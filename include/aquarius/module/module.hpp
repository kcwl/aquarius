#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <typename T>
	class _module : public basic_module<T>
	{
		using base = basic_module<T>;

	public:
		using typename base::core_type;

	public:
		_module(const std::string& name)
			: base(name)
		{}

		virtual ~_module() = default;

	public:
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

		virtual void timer(std::chrono::milliseconds) override
		{
			return;
		}

		virtual auto run() -> asio::awaitable<void> override
		{
			co_return;
		}
	};
} // namespace aquarius