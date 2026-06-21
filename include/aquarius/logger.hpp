#pragma once
#include <aquarius/detail/config.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/registration.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/record_ordering.hpp>
#include <boost/log/utility/setup/console.hpp>

#ifdef _DEBUG
#define LOGGER_LEVEL trivial::debug
#else
#define LOGGER_LEVEL trivial::info
#endif

namespace aquarius
{
	namespace logging = boost::log;
	namespace attrs = boost::log::attributes;
	namespace src = boost::log::sources;
	namespace sinks = boost::log::sinks;
	namespace expr = boost::log::expressions;
	namespace keywords = boost::log::keywords;
	namespace trivial = boost::log::trivial;

	BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "Line", uint32_t)
	BOOST_LOG_ATTRIBUTE_KEYWORD(file_name, "File", attrs::mutable_constant<std::string>)

	struct logger
	{
		using file_sink = sinks::asynchronous_sink<sinks::text_file_backend>;

		explicit logger()
		{
			static std::once_flag flag{};
			std::call_once(flag, &logger::init_logger, this);
		}

		void init_logger()
		{
			auto file_sink_ptr = boost::make_shared<file_sink>(
				keywords::file_name = "file.log", keywords::target_file_name = "%Y%m%d_%H%M%S.%5N.log",
				keywords::registration = 16384,
				keywords::order = logging::make_attr_ordering<uint32_t>("RecordID", std::less<uint32_t>()));

			file_sink_ptr->locked_backend()->set_file_collector(
				sinks::file::make_collector(keywords::target = "logs/", keywords::max_size = 16 * 1024 * 1024,
											keywords::max_files = 512, keywords::min_free_space = 100 * 1024 * 1024));

			file_sink_ptr->set_formatter(expr::format("[%1%][%2%:%3%]<%4%> %5%") %
										 expr::attr<trivial::severity_level>("Severity") %
										 expr::attr<std::string>("File") % expr::attr<uint32_t>("Line") %
										 expr::attr<boost::posix_time::ptime>("TimeStamp") % expr::smessage);

			file_sink_ptr->set_filter(trivial::severity >= LOGGER_LEVEL);

			logging::add_console_log(std::clog,
									 keywords::format = "[%Severity%][%File%:%Line%]<%TimeStamp%> %Message%");

			logging::core::get()->add_sink(file_sink_ptr);

			logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
			logging::core::get()->add_global_attribute("RecordID", attrs::counter<uint32_t>());
			logging::core::get()->add_global_attribute("File", attrs::mutable_constant<std::string>(""));
			logging::core::get()->add_global_attribute("Line", attrs::mutable_constant<uint32_t>(0));
		}
	};
} // namespace aquarius

#define AQUARIUS_LOG_LGP(lvl)                                                                                          \
	BOOST_LOG_TRIVIAL(lvl) << boost::log::add_value("File", __FILE__) << boost::log::add_value("Line", __LINE__)

#define XLOG_TRACE() AQUARIUS_LOG_LGP(trace)
#define XLOG_DEBUG() AQUARIUS_LOG_LGP(debug)
#define XLOG_INFO() AQUARIUS_LOG_LGP(info)
#define XLOG_WARNING() AQUARIUS_LOG_LGP(warning)
#define XLOG_ERROR() AQUARIUS_LOG_LGP(error)
#define XLOG_FATAL() AQUARIUS_LOG_LGP(fatal)

[[maybe_unused]] inline static aquarius::logger __auto_init_log;