#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/ini/parse.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <typename T, typename Config>
	class _module : public basic_module<T>
	{
		using base = basic_module<T>;

	public:
		using typename base::core_type;
		using config_type = Config;
		constexpr static std::string_view config_path = config_type::path;

	public:
		_module(io_context& io, const std::string& name)
			: base(io, name)
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
				XLOG_ERROR() << "module[" << this->name() << "] config file not exsit! path:" << path;

				return false;
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
			return true;
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

	struct no_config
	{
		constexpr static std::string_view path = "empty"sv;
	};

	template <typename Core>
	class no_config_module : public _module<Core, no_config>
	{
	public:
		no_config_module(io_context& io, const std::string& name)
			: _module<Core, no_config>(io, name)
		{}

	public:
		virtual bool config() override
		{
			return true;
		}
	};
} // namespace aquarius