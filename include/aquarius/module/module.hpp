#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/ini/parse.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{

	template <typename Core, typename Config>
	class _module : public basic_module<Core>
	{
	public:
		using typename basic_module<Core>::core_type;
		using config_type = Config;
		constexpr static std::string_view config_path = config_type::path;

	public:
		_module(const std::string& name)
			: basic_module<Core>(name)
		{}

		virtual ~_module() = default;

	public:
		virtual bool config() override
		{
			error_code ec{};
			config_ = ini::parse<config_type>(std::string(config_path.data()), ec);

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

		virtual bool enable() override
		{
			return true;
		}

		virtual void timer(std::chrono::milliseconds) override
		{
			return;
		}

		virtual auto run(io_context&) -> awaitable<void>
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

	template<typename Core>
	using no_config_module = _module<Core, no_config>;
} // namespace aquarius