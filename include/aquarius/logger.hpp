#pragma once
#include <aquarius/detail/config.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/registration.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/record_ordering.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <filesystem>
#include <source_location>
#include <string>

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

	template <typename T>
	auto set_attribute_file_name(const std::string& file_name)
	{
		auto attr_file =
			logging::attribute_cast<attrs::mutable_constant<T>>(logging::core::get()->get_global_attributes()["File"]);
		attr_file.set(file_name);

		return attr_file.get();
	}
	template <typename T>
	auto set_attribute_line(uint32_t line)
	{
		auto attr_line =
			logging::attribute_cast<attrs::mutable_constant<T>>(logging::core::get()->get_global_attributes()["Line"]);
		attr_line.set(line);

		return attr_line.get();
	}
} // namespace aquarius

#define XLOG_FILE_AND_LINE(lvl)                                                                                        \
	BOOST_LOG_STREAM_WITH_PARAMS(                                                                                      \
		::boost::log::trivial::logger::get(),                                                                          \
		(aquarius::set_attribute_file_name<std::string>(                                                               \
			std::filesystem::path(std::source_location::current().file_name()).filename().string()))(                  \
			aquarius::set_attribute_line<uint32_t>(std::source_location::current().line()))(                           \
			::boost::log::keywords::severity = ::boost::log::trivial::lvl))

#define XLOG_TRACE() XLOG_FILE_AND_LINE(trace)
#define XLOG_DEBUG() XLOG_FILE_AND_LINE(debug)
#define XLOG_INFO() XLOG_FILE_AND_LINE(info)
#define XLOG_WARNING() XLOG_FILE_AND_LINE(warning)
#define XLOG_ERROR() XLOG_FILE_AND_LINE(error)
#define XLOG_FATAL() XLOG_FILE_AND_LINE(fatal)