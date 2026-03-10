#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/ini/parse.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	struct no_config
	{
		constexpr static std::string_view path = "empty"sv;

		bool enable()
		{
			return true;
		}
	};
	template <typename T, typename Config = no_config>
	class _module : public basic_module<T>
	{
		using base = basic_module<T>;

	public:
		using typename base::core_type;
		using config_type = Config;
		constexpr static std::string_view config_path = config_type::path;

	public:
		_module(const std::string& name)
			: base(name)
		{}

		virtual ~_module() = default;

	public:
		virtual bool config() override
		{
			error_code ec{};

			auto path = std::filesystem::current_path().append(config_path);
			path += ".ini";

			if (!std::filesystem::exists(path))
			{
				XLOG_INFO() << "module[" << this->name() << "] config file not exsit and then skip! path:" << path;

				return true;
			}

			try
			{
				config_ = ini::parse<config_type>(path.string(), ec);
			}
			catch (...)
			{
				XLOG_ERROR() << "module[" << this->name() << "] config structure error!";

				return false;
			}

			return !ec;
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

		virtual bool enable() override
		{
			return config_.enable();
		}

		virtual void timer(std::chrono::milliseconds) override
		{
			return;
		}

		virtual auto run() -> awaitable<void> override
		{
			co_return;
		}

	private:
		Config config_;
	};
} // namespace aquarius