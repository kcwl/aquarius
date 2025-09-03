#pragma once
#include <iostream>
#include <ranges>
#include <vector>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>

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

			template <typename BufferSequence>
			void commit(BufferSequence& buffer)
			{}

			bool consume(std::span<char> buffer)
			{
				if (buffer.empty())
					return false;

				auto req_line_views = buffer | std::views::split(' ');

				auto iter = req_line_views.begin();

				if (iter == req_line_views.end())
					return false;

				parse_method(std::string_view(*iter), method_);

				if (++iter == req_line_views.end())
					return false;

				auto ec = parse_uri(std::string_view(*iter), path_, querys_);

				if (++iter == req_line_views.end())
					return false;

				if (!ec)
					return false;

				return parse_version(std::string_view(*iter), version_);
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
			void parse_method(std::string_view method, http_method& m)
			{
				if (method == "POST"sv)
				{
					m = http_method::post;
				}
				else if (method == "GET"sv)
				{
					m = http_method::get;
				}
			}

			bool parse_uri(std::string_view buf, std::string& paths,
						   std::vector<std::pair<std::string, std::string>>& querys)
			{
				auto iter = buf.begin();

				while (iter != buf.end())
				{
					if (*iter == '/')
					{
						parse_path(iter, buf.end(), paths);
					}
					else if (*iter == '?' || *iter == '&')
					{
						querys.push_back({});
						parse_querys(iter, buf.end(), querys.back());
					}
					else if (*iter == '#' || *iter == ' ')
					{
						break;
					}
					else
					{
						return false;
					}
				}

				return true;
			}

			bool parse_version(std::string_view buf, http_version& version)
			{
				auto iter = std::find_if(buf.begin(), buf.end(), [](const auto c) { return c == '.'; });

				std::string ver(buf.data(), buf.size());

				if (iter == buf.end())
				{
					if (ver == "HTTP2")
					{
						version = http_version::http2;
					}
					else if (ver == "HTTP3")
					{
						version = http_version::http3;
					}
					else
					{
						return false;
					}
				}
				else
				{
					auto suffix = buf.substr(std::distance(buf.begin(), iter) + 1);

					if (std::atoi(suffix.data()) == 1)
					{
						version = http_version::http1_1;
					}
					else if (std::atoi(suffix.data()) == 0)
					{
						version = http_version::http1_0;
					}
					else
					{
						return false;
					}
				}

				return true;
			}

			template <typename Iterator>
			bool parse_path(Iterator& begin, Iterator end, std::string& path)
			{
				if (*begin++ != '/')
					return false;

				path.push_back('/');

				while (begin != end)
				{
					if (!std::isalnum(*begin) && (*begin != '_'))
						break;

					path.push_back(*begin++);
				}

				return true;
			}

			template <typename Iterator>
			bool parse_querys(Iterator& begin, Iterator end, std::pair<std::string, std::string>& query)
			{
				if (*begin != '?' && *begin != '&')
					return false;

				begin++;

				while (begin != end)
				{
					if (!std::isalnum(*begin) && (*begin != '_'))
						break;

					query.first.push_back(*begin++);
				}

				if (begin == end)
					return false;

				if (*begin != '=')
					return false;

				begin++;

				while (begin != end)
				{
					if (!std::isalnum(*begin) && (*begin != '_'))
						break;

					query.second.push_back(*begin++);
				}

				return true;
			};

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

			std::string_view reason() const
			{
				return reason_;
			}

			std::string_view& reason()
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

			template <typename BufferSequence>
			void commit(BufferSequence& buffer)
			{
				auto header_str = version_to_string(version_);
				header_str.push_back(' ');
				header_str += std::to_string(static_cast<int>(status_)) + ' ';
				header_str += get_http_status_string(status_);
				header_str += "\r\n";

				std::copy(header_str.begin(), header_str.end(), std::back_inserter(buffer));
			}

			template <typename BufferSequence>
			void consume(BufferSequence& buffer)
			{}

		private:
			http_status status_;

			std::string_view reason_;

			http_version version_;
		};
	} // namespace virgo
} // namespace aquarius