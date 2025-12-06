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

			virtual ~http_fields() = default;

		public:
			http_fields(const http_fields& other)
				: fields_(other.fields_)
			{}

			http_fields& operator=(const http_fields& other)
			{
				if (this != std::addressof(other))
				{
					fields_ = other.fields_;
				}

				return *this;
			}

			http_fields(http_fields&& other) noexcept
				: fields_(std::move(other.fields_))
			{}

			http_fields& operator=(http_fields&& other) noexcept
			{
				if (this != std::addressof(other))
				{
					fields_ = std::move(other.fields_);
				}

				return *this;
			}

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

			// void set_field(const std::string& f, const std::string& v)
			//{
			//	fields_[f] = v;
			// }
			void set_field(std::string_view f, std::string_view v)
			{
				fields_[std::string(f.data(), f.size())] = std::string(v.data(), v.size());
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

			void move_copy(http_fields hf)
			{
				*this = std::move(hf);
			}

			std::map<std::string, std::string> fields() const
			{
				return fields_;
			}

		protected:
			std::map<std::string, std::string> fields_;
		};
	} // namespace virgo
} // namespace aquarius