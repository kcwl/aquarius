#pragma once
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <source_location>
#include <string>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace trivial = boost::log::trivial;

namespace aquarius
{
	inline void init_logger()
	{
		using file_sink = sinks::synchronous_sink<sinks::text_file_backend>;

		auto sink_ptr = boost::make_shared<file_sink>(keywords::file_name = "file.log",
													  keywords::target_file_name = "%Y%m%d_%H%M%S_%5N.log",
													  keywords::registration = 16384);

		sink_ptr->locked_backend()->set_file_collector(
			sinks::file::make_collector(keywords::target = "logs/", keywords::max_size = 16 * 1024 * 1024,
										keywords::max_files = 512, keywords::min_free_space = 100 * 1024 * 1024));

		sink_ptr->set_formatter(expr::format("[%1%]:[%2%]<%3%> %4%") %
								expr::attr<attrs::current_thread_id::value_type>("ThreadID") %
								expr::attr<boost::posix_time::ptime>("TimeStamp") %
								expr::attr<trivial::severity_level>("Severity") % expr::smessage);

		sink_ptr->set_filter(trivial::severity >= trivial::info);

		logging::core::get()->add_sink(sink_ptr);

		logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
		logging::core::get()->add_global_attribute("RecordID", attrs::counter<uint32_t>());
		logging::core::get()->add_global_attribute("ThreadID", attrs::current_thread_id());
		logging::core::get()->add_global_attribute("File", attrs::mutable_constant<std::string>(""));
		logging::core::get()->add_global_attribute("Line", attrs::mutable_constant<int>(0));
	}

	struct file_name
	{
		static std::string substr(const std::string& path)
		{
			auto pos = path.find_last_of("/\\");

			return path.substr(pos + 1);
		}
	};

} // namespace aquarius

#define XLOG(level)                                                                                                    \
	BOOST_LOG_TRIVIAL(level) << "[" << aquarius::file_name::substr(std::source_location::current().file_name()) << ":" << std::source_location::current().line() << "] "

#define XLOG_TRACE() XLOG(trace)
#define XLOG_DEBUG() XLOG(debug)
#define XLOG_INFO() XLOG(info)
#define XLOG_WARNING() XLOG(warning)
#define XLOG_ERROR() XLOG(error)
#define XLOG_FATAL() XLOG(fatal)
