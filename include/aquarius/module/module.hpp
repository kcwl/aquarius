#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/ini/parse.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <typename Core, typename Config, detail::string_literal ConfigPath>
	class _module : public basic_module<Core>
	{
	public:
		using typename basic_module<Core>::core_type;
		using config_type = Config;
		constexpr static auto config_path = ConfigPath;

	public:
		_module(const std::string& name)
			: basic_module<Core>(name)
		{}

		virtual ~_module() = default;

	public:
		virtual bool config() override
		{
			error_code ec{};
			config_ = ini::parse<Config>(std::string(detail::bind_param<ConfigPath>::value), ec);

			if (!ec)
				return true;

			XLOG_ERROR() << "parse config error: " << ec.message();

			throw ec;
		}

		Config configs() const
		{
			return config_;
		}

		virtual bool init() override
		{
			return true;
		}

		virtual void stop() override
		{
			return;
		}

		virtual void timer(std::chrono::milliseconds) override
		{
			return;
		}

		virtual auto run() -> awaitable<void>
		{
			co_return;
		}

	protected:
		virtual std::shared_ptr<core_type> core() override
		{
			return nullptr;
		}

	private:
		Config config_;
	};
} // namespace aquarius