#pragma once
#include <iostream>
#include <map>
#include <string>

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
			std::string find(const std::string& f) const
			{
				auto iter = fields_.find(f);

				if (iter == fields_.end())
					return {};

				return iter->second;
			}

			void set_field(const std::string& f, const std::string& v)
			{
				fields_[f] = v;
			}

			bool has_content_length() const
			{
				return !find("Content-Length").empty();
			}

			void content_length(uint64_t len)
			{
				fields_["Content-Length"] = std::string_view(std::to_string(len).c_str());
			}

			uint64_t content_length()
			{
				auto iter = fields_.find("Content-Length");
				if (iter == fields_.end())
					return 0;

				return std::atoi(iter->second.data());
			}

			bool keep_alive()
			{
				auto iter = fields_.find("Connection");
				if (iter == fields_.end())
					return false;

				return iter->second == "keep-alive" ? true : false;
			}

			void keep_alive(bool k)
			{
				fields_["Connection"] = k ? "keep-alive" : "close";
			}

		protected:
			std::map<std::string, std::string> fields_;
		};
	} // namespace virgo
} // namespace aquarius