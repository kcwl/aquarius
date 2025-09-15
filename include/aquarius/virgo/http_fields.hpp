#pragma once
#include <iostream>
#include <string_view>
#include <map>
#include <string>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace virgo
	{
		class http_fields
		{
		public:
			http_fields() = default;

		public:
			std::ostream& operator<<(std::ostream& os) const
			{
				for (auto& s : fields_)
				{
					os << s.first << ":" << s.second << "\r\n";
				}

				os << "\r\n";

				return os;
			}

		public:
			std::string_view find(std::string_view f) const
			{
				auto iter = fields_.find(f);

				if (iter == fields_.end())
					return {};

				return iter->second;
			}

			template <typename T>
			void set_field(std::string_view f, T v)
			{
				std::stringstream ss;
				ss << v;
				fields_[f] = std::string_view(ss.str().c_str());
			}

			bool has_content_length() const
			{
				return !find("Content-Length"sv).empty();
			}

			void content_length(uint64_t len)
			{
				fields_["Content-Length"sv] = std::string_view(std::to_string(len).c_str());
			}

			uint64_t content_length()
			{
				auto iter = fields_.find("Content-Length"sv);
				if (iter == fields_.end())
					return 0;

				return std::atoi(iter->second.data());
			}

			bool keep_alive()
			{
				auto iter = fields_.find("Connection"sv);
				if (iter == fields_.end())
					return false;

				return iter->second == "keep-alive"sv ? true : false;
			}

			void keep_alive(bool k)
			{
				fields_["Connection"sv] = k ? "keep-alive"sv : "close"sv;
			}

		protected:
			std::map<std::string_view, std::string_view> fields_;
		};
	} // namespace virgo
} // namespace aquarius