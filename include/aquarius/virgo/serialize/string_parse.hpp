#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/virgo/parse_helper.hpp>
#include <string>
#include <boost/pfr.hpp>
#include <aquarius/virgo/http_status.hpp>

namespace aquarius
{
	namespace virgo
	{

		struct string_parse
		{
			template <typename T>
			std::string to_datas(T&& value)
			{
				std::stringstream ss{};
				boost::pfr::for_each_field_with_name(std::forward<T>(value), [&](auto&& name, auto&& field)
													 { ss << name << ": " << field << "\n"; });

				return ss.str();
			}

			template<typename T>
			void to_datas(T&& value, detail::flex_buffer<char>& buffer)
			{
				std::stringstream ss{};
				boost::pfr::for_each_field_with_name(std::forward<T>(value), [&] (auto&& name, auto&& field)
													 {
														 ss << name << ": " << field << "\n";
													 });

				std::copy(ss.str().begin(), ss.str().end(), buffer.wdata());

				buffer.commit(ss.str().size());
			}

			template <typename T>
			T from_datas(detail::flex_buffer<char>& buffer)
			{
				auto from_datas_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					return T{ (parse_line<I, T>(buffer), ...)  };
				};

				return from_datas_impl(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});
			}

		private:
			template<std::size_t I, typename T>
			auto parse_line(detail::flex_buffer<char>& buffer)
			{
				std::string result_value{};

				auto result = read_key<T, ':'>(buffer).and_then(
					[&] (const auto& key) -> std::expected<bool, error_code>
					{
						if (key.empty())
							return std::unexpected(make_error_code(virgo::http_status::ok));

						if (key != boost::pfr::get_name<I, T>())
							return std::unexpected(make_error_code(virgo::http_status::bad_request));

						return read_value<T, '\r'>(buffer).and_then(
							[&] (const auto& value) -> std::expected<bool, error_code>
							{
								result_value = value;
								buffer.consume(1);
								return true;
							});
					});

				if (!result.has_value())
					return T{};

				return result_value;
			}
		};
	} // namespace detail
} // namespace aquarius