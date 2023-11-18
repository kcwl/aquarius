#pragma once
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace trivial = boost::log::trivial;



namespace aquarius
{
	BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, src::logger_mt)


	inline void init_logger()
	{
		using file_sink = sinks::asynchronous_sink<sinks::text_multifile_backend>;

		auto sink_ptr = boost::make_shared<file_sink>();

		sink_ptr->locked_backend()->set_file_name_composer(sinks::file::as_file_name_composer(
			expr::stream << "logs/" << expr::attr<boost::thread::id>("ThreadID") << ".log"));

		sink_ptr->set_formatter(expr::format("%1%: [%2%][%3%] - %4%") % expr::attr<uint32_t>("RecordID") %
								expr::attr<boost::posix_time::ptime>("TimeStamp") % expr::attr<trivial::severity_level>("Severity") %
								expr::smessage);

		sink_ptr->set_filter(trivial::severity >= trivial::info);

		logging::core::get()->add_sink(sink_ptr);

		logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
		logging::core::get()->add_global_attribute("RecordID", attrs::counter<uint32_t>());
	}
} // namespace aquarius

#define XLOG(level) BOOST_LOG_TRIVIAL(aquarius::logger::get(), trivial::##level)