#pragma once
#include <iostream>
#include <ranges>
#include <vector>
#include <expected>
#include <format>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace virgo
	{
		template <bool Request>
		class http_header;

		template <>
		class http_header<true>
		{
		public:
			http_header() = default;

		public:
			std::ostream operator<<(std::ostream& os) const
			{
				os << method_ << " " << path_;

				if (!querys().empty())
				{
					os << "?";
				}

				for (auto& s : querys())
				{
					os << s.first << "=" << s.second;

					os << "&";
				}

				os.seekp(-1, std::ios::cur);

				os << version_ << "\r\n";
			}

			template <typename T>
			std::expected<bool, std::string> commit(detail::flex_buffer<T>& buffer)
			{
				std::string path = path_;
				if (!querys().empty())
				{
					path += "?";

					for (auto& s : querys())
					{
						path += s.first + "=" + s.second;
						path += "&";
					}

					path.substr(0, path.size() - 1);
				}

				std::string header_str =
					std::format("{} {} {}\r\n", from_method_string(method_), path, from_version_string(version_));

				if (header_str.size() > buffer.remain())
				{
					return std::unexpected("buffer is not enough");
				}

				std::memcpy(buffer.wdata(), header_str.c_str(), header_str.size());

				buffer.commit(header_str.size());

				return true;
			}

			template <typename T>
			std::expected<bool, error_code> consume(detail::flex_buffer<T>& buffer)
			{
				auto result = parse_method(buffer);

				if (!result.has_value())
					return result;

				result = parse_uri(buffer);

				if (!result.has_value())
					return result;

				result = parse_version(buffer);

				if (!result.has_value())
					return result;

				return true;
			}

		public:
			http_method method() const
			{
				return method_;
			}

			http_method& method()
			{
				return method_;
			}

			void method(http_method m)
			{
				method_ = m;
			}

			http_version version() const
			{
				return version_;
			}

			http_version& version()
			{
				return version_;
			}

			void version(http_version v)
			{
				version_ = v;
			}

			const std::string& path() const
			{
				return path_;
			}

			std::string& path()
			{
				return path_;
			}

			void path(std::string p)
			{
				path_ = std::move(p);
			}

			std::vector<std::pair<std::string, std::string>>& querys()
			{
				return querys_;
			}

			const std::vector<std::pair<std::string, std::string>>& querys() const
			{
				return querys_;
			}

			void querys(std::vector<std::pair<std::string, std::string>> q)
			{
				querys_ = std::move(q);
			}

		private:
			template <typename T>
			std::expected<bool, error_code> parse_method(detail::flex_buffer<T>& buffer)
			{
				return read_value<T, ' '>(buffer).and_then(
					[&](const auto& value) -> std::expected<bool, error_code>
					{
						if (value == "POST")
						{
							method_ = http_method::post;
						}
						else if (value == "GET")
						{
							method_ = http_method::get;
						}
						else
						{
							return std::unexpected(error_code(http_status::bad_request));
						}

						return true;
					});
			}

			template <typename T>
			std::expected<bool, error_code> parse_uri(detail::flex_buffer<T>& buffer)
			{
				return parse_path(buffer)
					.and_then(
						[&](char end)
						{
							querys_.push_back({});
							return parse_querys(buffer, querys_.back());
						})
					.and_then(
						[&](char end) -> std::expected<bool, error_code>
						{
							if (end == '#' || end == ' ')
								return true;

							return std::unexpected(error_code(http_status::bad_request));
						});
			}

			template <typename T>
			std::expected<bool, error_code> parse_version(detail::flex_buffer<T>& buffer)
			{
				return read_value<T, '\r'>(buffer).and_then(
					[&](const auto& value) -> std::expected<bool, error_code>
					{
						auto pos = value.find('.');
						if (pos == std::string::npos)
						{
							if (value == "HTTP2")
							{
								version_ = http_version::http2;
							}
							else if (value == "HTTP3")
							{
								version_ = http_version::http3;
							}
							else
							{
								return std::unexpected(http_status::bad_request);
							}
						}
						else
						{
							auto suffix = value.substr(pos + 1);
							if (*suffix.data() == 1)
							{
								version_ = http_version::http1_1;
							}
							else
							{
								version_ = http_version::http1_0;
							}
						}

						return true;
					});
			}

			template <typename T>
			std::expected<char, error_code> parse_path(detail::flex_buffer<T>& buffer)
			{
				auto c = buffer.peek();

				if (c != '/')
					return std::unexpected(http_status::bad_request);

				path_.push_back('/');

				while (buffer.peek() != T(-1))
				{
					c = buffer.get();

					if (c == '?' || c == '#')
						break;

					if (!std::isalnum(c) && (c != '_'))
						return std::unexpected(http_status::bad_request);

					path_.push_back(c);
				}

				return c;
			}

			template <typename T>
			std::expected<char, error_code> parse_querys(detail::flex_buffer<T>& buffer,
														 std::pair<std::string, std::string>& query)
			{
				std::expected<char, error_code> result{};

				while (buffer.peek() != T(-1))
				{
					result = read_value<T, '='>(buffer)
								 .and_then(
									 [&](const auto& value)
									 {
										 query.first = value;

										 return read_value<T, '&', ' '>(buffer);
									 })
								 .and_then(
									 [&](const auto& value) -> std::expected<bool, error_code>
									 {
										 query.second = value;
										 return true;
									 });

					if (!result.has_value())
						break;

					if (result.value() == ' ')
						break;
				}

				return result;
			};

			template <typename T, char... SP>
			std::expected<std::string, error_code> read_value(detail::flex_buffer<T>& buffer)
			{
				std::string value{};
				while (buffer.peek() != T(-1))
				{
					auto c = buffer.get();
					if (((c == SP) || ...))
						return value;

					value.push_back(c);
				}

				return std::unexpected(http_status::bad_request);
			}

		private:
			http_method method_;

			http_version version_;

			std::string path_;

			std::vector<std::pair<std::string, std::string>> querys_;
		};

		template <>
		class http_header<false>
		{
		public:
			http_header() = default;

		public:
			http_status result() const
			{
				return status_;
			}

			http_status& result()
			{
				return status_;
			}

			void result(http_status s)
			{
				status_ = s;
			}

			std::string reason() const
			{
				return reason_;
			}

			std::string& reason()
			{
				return reason_;
			}

			void reason(std::string_view r)
			{
				reason_ = r;
			}

			http_version version() const
			{
				return version_;
			}

			http_version& version()
			{
				return version_;
			}

			void version(http_version v)
			{
				version_ = v;
			}

			template <typename T>
			std::expected<bool, error_code> commit(detail::flex_buffer<T>& buffer)
			{
				auto header_str = from_version_string(version_);
				header_str.push_back(' ');
				header_str += std::to_string(static_cast<int>(status_)) + ' ';
				header_str += get_http_status_string(status_);
				header_str += "\r\n";

				if (header_str.size() > buffer.remain())
				{
					return std::unexpected(http_status::bad_request);
				}

				std::memcpy(buffer.wdata(), header_str.c_str(), header_str.size());

				buffer.commit(header_str.size());

				return true;
			}

			template <typename T>
			std::expected<bool, error_code> consume(detail::flex_buffer<T>& buffer)
			{
				return read_value<' '>(buffer)
					.and_then(
						[&](const auto& value)
						{
							status_ = from_status_string(value);

							return read_value<' '>(buffer);
						})
					.and_then(
						[&](const auto& value)
						{
							version_ = from_version_string(value);

							return read_value<'\r'>(buffer);
						})
					.and_then(
						[&](const auto& value)
						{
							reason_ = value;

							return buffer.peek() == '\n' ? buffer.get() : std::unexpected(http_status::bad_request);
						});
			}

		private:
			http_status status_;

			std::string reason_;

			http_version version_;
		};
	} // namespace virgo
} // namespace aquarius