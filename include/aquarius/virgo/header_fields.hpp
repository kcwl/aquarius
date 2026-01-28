#pragma once
#include <iostream>
#include <map>
#include <string>

namespace aquarius
{
	namespace virgo
	{
		class header_fields
		{
		public:
			header_fields() = default;

			virtual ~header_fields() = default;

		public:
			header_fields(const header_fields& other)
				: fields_(other.fields_)
			{}

			header_fields& operator=(const header_fields& other)
			{
				if (this != std::addressof(other))
				{
					fields_ = other.fields_;
				}

				return *this;
			}

			header_fields(header_fields&& other) noexcept
				: fields_(std::move(other.fields_))
			{}

			header_fields& operator=(header_fields&& other) noexcept
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
			void set_header_fields(header_fields hf)
			{
				*this = std::move(hf);
			}

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

			std::map<std::string, std::string> fields() const
			{
				return fields_;
			}

			std::string content_type() const
			{
				return find("content-type");
			}

			uint32_t seq_number()
			{
				auto value = find("seq_number");

				if (value.empty())
					return static_cast<uint32_t>(-1);

				return to_integer<uint32_t>(value);
			}

			void seq_number(uint32_t v)
			{
				this->set_field("seq_number", std::to_string(v));
			}

			uint32_t content_length()
			{
				auto value = find("content-length");
				if (value.empty())
					return 0;

				return to_integer<uint32_t>(value);
			}

			void content_length(uint32_t v)
			{
				this->set_field("content-length", std::to_string(v));
			}

		private:
			template<typename T>
			T to_integer(const std::string& value)
			{
				std::stringstream ss{};
				ss << value;

				T result;

				ss >> result;

				return result;
			}

		protected:
			std::map<std::string, std::string> fields_;
		};
	} // namespace virgo
} // namespace aquarius