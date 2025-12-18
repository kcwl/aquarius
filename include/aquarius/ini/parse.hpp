#pragma once
#include <aquarius/error_code.hpp>
#include <boost/pfr.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <aquarius/detail/struct_name.hpp>

namespace aquarius
{
	namespace ini
	{
		template <typename T>
		T parse(const std::string& filepath, error_code& ec)
		{
			boost::property_tree::ptree pt;

			boost::property_tree::ini_parser::read_ini(filepath, pt);

			constexpr auto size = boost::pfr::tuple_size<T>::value;

			constexpr auto name = detail::struct_name<T>::value;

			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{
				return T{ pt.get<boost::pfr::tuple_element_t<I, T>>(name.data() + "." +
																	boost::pfr::get_name<I, T>())... };
			};

			return f(std::make_index_sequence<size>());
		}
	} // namespace ini
} // namespace aquarius